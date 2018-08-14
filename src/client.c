#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <malloc.h>
#include "../include/server_controls.h"

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

const int current_player = 0;

gamefield_t gamefield;
struct player* playerss;
    xy_t coord = {200,200};   
    

//SDL staff    
    
//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

SDL_Event e;
bool quit = false;

//---

int create_test_players(int count);
int create_test_pellets(int count);


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


bool init()
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
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

void close()
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


void main_loop()
{
    int i = 0;
	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}
	}

	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);
        
        do
        {
            fill_circle(gRenderer, gamefield.players[i].position.x,
                        gamefield.players[i].position.y, gamefield.players[i].size, 0xFF, 0x00, 0xFF, 0xFF);
            ++i;
        }
        while(i<gamefield.players_count);
        
        i=0;
        draw_rect(gRenderer, gamefield.pellets[0].position, gamefield.pellets[0].size);
        do
        {
            draw_rect(gRenderer, gamefield.pellets[i].position, gamefield.pellets[i].size);
            ++i;
        }
        while(i<gamefield.players_count);

	//Update screen
	SDL_RenderPresent(gRenderer);
}

void create_gamefield(xy_t gsize)
{
    gamefield.size.x = gsize.x;
    gamefield.size.y = gsize.y;
    
    create_test_players(gamefield.players_count);
    create_test_pellets(gamefield.pellets_count);
}

int create_test_players(int count)
{
    int i=0, r = 15;
    
    do
    {
        gamefield.players[i].color = i;
        gamefield.players[i].size = r;
        gamefield.players[i].position.x = rand() % (gamefield.size.x - r) + r;
        gamefield.players[i].position.y = rand() % (gamefield.size.y - r) + r;
               
        ++i;
    }while(i < 8);
}

int create_test_pellets(int count)
{
    int i=0;
    
    do
    {
        gamefield.pellets[i].color = i;
        gamefield.pellets[i].size = 10;
        gamefield.pellets[i].position.x = rand() % 340 + 30;
        gamefield.pellets[i].position.y = rand() % 340 + 30;
               
        ++i;
    }while(i < 8);
}


int main()
{
    xy_t coord;
    int i=0;    
    coord.x = 1024;
    coord.y = 768;
    
    srand(time(0));
    
    gamefield.players_count = 8;
    gamefield.pellets_count = 10;
    gamefield.players = (player_t *)malloc(sizeof(struct player ) * gamefield.players_count);
    gamefield.pellets = (pellet_t *)malloc(sizeof(pellet_t) * gamefield.pellets_count);
    
    create_gamefield(coord);
    
    //Start up SDL and create window
    if (!init())
    {
            printf("Failed to initialize!\n");
    }
    else
    {
        while (!quit)
        {
            main_loop();
        }
        
    }

    //Free resources and close SDL
    close();
    
 
    return 0;
}