#include "../include/server_controls.h"
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

game_parameters_t game_parameters;
gamefield_t* gamefield_create(int width, int height)
{
	srand(time(NULL));
	game_parameters.def_size = 10;
	game_parameters.def_speed = 1.0;
	game_parameters.def_drag = 0.2;

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
	player_t player;
	player.color = 0; ///\todo Random color.
	player.size = game_parameters.def_size;
	player.position.x = rand()%gamefield->size.x;
	player.position.y = rand()%gamefield->size.y;
	player.speed.x = 0;
	player.speed.y = 0;
	player.drag = 0;
	gamefield->players[player_id] = player;
	gamefield->players_count++;
	return player_id;
}