#include "../include/server_controls.h"

#define SERVER_CONTROLS_THREADS_COUNT 3
game_parameters_t game_parameters;
pthread_t *tid;

int gamefield_free(gamefield_t* gamefield)
{
	int i;
	for(i = 0; i < SERVER_CONTROLS_THREADS_COUNT; i++)
	{
		pthread_cancel(tid[i]);
		pthread_join(tid[i], NULL);
	}
	free(tid);
	free(gamefield->players);
	free(gamefield->pellets);
	free(gamefield);
	return 0;
}

gamefield_t* gamefield_create(int width, int height)
{
	srand(time(NULL));
	game_parameters.def_player_size = 10;
	game_parameters.def_pellet_size = 5;
	game_parameters.def_speed = 1.0;
	game_parameters.def_accel = 0.4;
	game_parameters.def_drag = 0.2;
	game_parameters.player_pace = 200000;
	game_parameters.pellet_pace = 1000000;

	gamefield_t *gamefield = malloc(sizeof(gamefield_t));
	xy_t size = {width, height};
	gamefield->size = size;
	gamefield->players = malloc(0);
	gamefield->players_count = 0;
	gamefield->pellets = malloc(0);
	gamefield->pellets_count = 0;
	return gamefield;
}

int gamefield_add(gamefield_t* gamefield)
{
	int player_id = gamefield->players_count;
	gamefield->players = realloc(gamefield->players, (gamefield->players_count + 1) * sizeof(player_t));
	if(gamefield->players == NULL) return -1;
	player_t player;
	player.color = 0; ///\todo Random color.
	player.size = game_parameters.def_player_size;
	player.position.x = rand()%gamefield->size.x;
	player.position.y = rand()%gamefield->size.y;
	player.speed.x = 0;
	player.speed.y = 0;
	player.drag = 0;
	gamefield->players[player_id] = player;
	gamefield->players_count++;
	return player_id;
}

void *cycle_make_pellet(void* v_gamefield)
{
	gamefield_t *gamefield = v_gamefield;
	while(1)
	{
		gamefield->pellets = realloc(gamefield->pellets, (gamefield->pellets_count + 1) * sizeof(pellet_t));
		pellet_t pellet;
		pellet.color = 0; ///\todo Color.
		pellet.size = game_parameters.def_pellet_size;
		pellet.position.x = rand()%gamefield->size.x;
		pellet.position.y = rand()%gamefield->size.y;
		gamefield->pellets[gamefield->pellets_count] = pellet;
		gamefield->pellets_count++;
		usleep(game_parameters.pellet_pace);
		pthread_testcancel();
	}
}

void *cycle_move(void* v_gamefield)
{
	int i;
	gamefield_t *gamefield = v_gamefield;
	while(1)
	{
		for(i = 0; i < gamefield->players_count; i++)
		{
			gamefield->players[i].position.x += gamefield->players[i].speed.x;
			gamefield->players[i].position.y += gamefield->players[i].speed.y;
		}
		usleep(game_parameters.player_pace);
		pthread_testcancel();
	}
}

struct gamefield_and_events {
	gamefield_t *gamefield;
	event_t *event_out;
	event_t *event_in;
};

void *cycle_controls(void* v_gamefield_and_events)
{
	struct gamefield_and_events *gamefield_and_events = v_gamefield_and_events;
	gamefield_t *gamefield = gamefield_and_events->gamefield;
	event_t* event_out = gamefield_and_events->event_out;
	event_t* event_in = gamefield_and_events->event_in;
}

int gamefield_start(gamefield_t* gamefield, event_t* event_out, event_t* event_in)
{
	tid = malloc(SERVER_CONTROLS_THREADS_COUNT * sizeof(pthread_t));
	if(tid == NULL)
		return -1;
	if(pthread_create(&tid[0], NULL, cycle_make_pellet, gamefield) > 0)
		return -1;
	if(pthread_create(&tid[1], NULL, cycle_move, gamefield) > 0)
		return -1;
	struct gamefield_and_events gamefield_and_events;
	gamefield_and_events.gamefield = gamefield;
	gamefield_and_events.event_out = event_out;
	gamefield_and_events.event_in = event_in;
	if(pthread_create(&tid[2], NULL, cycle_controls, &gamefield_and_events) > 0)
		return -1;
	return 0;
}