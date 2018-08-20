//Main with sending events from server_controls.c when button pressed
#include "../include/server_controls.h"
#include "../include/server_networking.h"
#include "../include/client_networking.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

typedef struct rgb_color
{
    int r;
    int g;
    int b;
    int a;
    
} color_t;

SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

SDL_Event e;
bool quit = false;

color_t get_rgb_color(int color_num)
{
    int n = color_num * 1000;
    
    color_t rgb;
    
    rgb.r = (int)(n / 13) % 256;
    rgb.g = (int)(n / 19) % 256;
    rgb.b = (int)(n / 23) % 256;
    
    rgb.a=0;
    
    return rgb;
}

void fill_circle(SDL_Renderer *surface, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	// Note that there is more to altering the bitrate of this 
	// method than just changing this value. 
	static const int BPP = 4;

	//double ra = (double)radius;

	for (double dy = 1; dy <= radius; dy += 1.0)
	{

		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		int x = cx - dx;
		SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
		SDL_RenderDrawLine(gRenderer, cx - dx, cy + dy - radius, cx + dx, cy + dy - radius);
		SDL_RenderDrawLine(gRenderer, cx - dx, cy - dy + radius, cx + dx, cy - dy + radius);
	}
}

void draw_rect(SDL_Renderer *surface, xy_t coordinates, int size)
{
    SDL_Rect fillRect = { coordinates.x, coordinates.y, size, size };
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );        
    SDL_RenderFillRect( gRenderer, &fillRect );
}


bool initmy()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			}
		}
	}

	return success;
}

void closemy()
{
	//Free loaded images
	//gSpriteSheetTexture.free();

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void main_loop(void *v_gamefield, int player_id)
{
	gamefield_t *gamefield = (gamefield_t *)v_gamefield;
    int i = 0;
    color_t player_color;

	event_t event;
	event.event_id = EVENT_PLAYER_MOVE;
	event.arg_x = player_id;
	event.next = NULL;
	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}
		if ( e.type == SDL_KEYUP )
		{
			event.arg_y = 0;
			send_event(event);
		}
		if ( e.type == SDL_KEYDOWN )
                {
                    
                    // TODO:
                    // Send to server key pressed event
                    
                    switch(e.key.keysym.sym)
                    {
                        case SDLK_UP:
                            event.arg_y = 1;
                            send_event(event);
                            break;
                        
                        case SDLK_DOWN:
                            event.arg_y = 2;
                            send_event(event);
                            break;
                        case SDLK_LEFT:
                            event.arg_y = 3;
                            send_event(event);
                            break;
                            
                        case SDLK_RIGHT:
                            event.arg_y = 4;
                            send_event(event);
                            break;
                            
                            
                        default:
                            break;
                    }
                }
	}

	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
        
        do
        {
        	if(gamefield->players[i].alive){
            player_color = get_rgb_color(i+1);
            fill_circle(gRenderer, gamefield->players[i].position.x,
                        gamefield->players[i].position.y, gamefield->players[i].size, 
                        player_color.r, player_color.g, player_color.b, 0xFF);
        	}
        	++i;
        }
        while(i<gamefield->players_count);
        
        i=0;
        draw_rect(gRenderer, gamefield->pellets[0].position, gamefield->pellets[0].size);
        do
        {
            draw_rect(gRenderer, gamefield->pellets[i].position, gamefield->pellets[i].size);
            ++i;
        }
        while(i<gamefield->pellets_count);

	//Update screen
	SDL_RenderPresent(gRenderer);
}

int main()
{
	gamefield_t* gamefield = gamefield_create();
	if(gamefield==NULL) return -1;
	//gamefield_add(gamefield);
	//gamefield_add(gamefield);
	event_t *event_out = NULL;
	event_t *event_in = NULL;
	gamefield_start(gamefield, &event_out, &event_in);
	networking_start(gamefield, &event_out, &event_in);
	int player = connect_to_server();
	if (!initmy())
    {
            printf("Failed to initialize!\n");
    }
    else
    {
        while (!quit)
        {
            main_loop(gamefield, player);
            event_t event = listen_event();
            if(event.event_id != -1) printf("%d\n", event.event_id);
        }
        
    }

    //Free resources and close SDL
    closemy();
	gamefield_free(gamefield);
	networking_free();
	return 0;
}