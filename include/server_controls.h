#include <pthread.h>
#include <malloc.h>

#ifndef SERVER_CONTROLS_H
#define SERVER_CONTROLS_H

///Event id when player eaten by another.\warning Need arg of type struct xy_t when used! x - player eaten id and y - player ate id.
#define EVENT_PLAYER_EATEN 1
///Event id when player ate a pellet.\warning Need arg of type struct xy_t when used! x - pellet eaten id and y - pellet ate id.
#define EVENT_PELLET_EATEN 2
///Event id when player change size.\warning Need arg of type struct xy_t when used! x - player id and y - size.\todo Shoot yourself into players.
#define EVENT_PLAYER_SIZE 1
///Event id when player moves.\warning Need arg of type int when used!
#define EVENT_PLAYER_MOVE 3

///Struct for two values type of int.\note Used for coordinates, speed, or as argument for event.
typedef struct xy {
	int x; ///<	First value.
	int y; ///<	Second value.
} xy_t;

///Struct containing global game parameters.
typedef struct game_parameters {
	int def_size; ///<	Default player size.
	float def_speed; ///<	Default player speed.
	float def_drag; ///<	Default drag force.
} game_parameters_t;

///Struct containing player parameters.
typedef struct player {
	int color; ///<		Player color.\todo Color type.
	int size; ///<		Player size.
	xy_t position; ///<	Player position on the field.
	xy_t speed; ///<	Player speed in X, Y direction.
	float drag; ///<	How much force slows player down.
} player_t;

///Struct containing player parameters.
typedef struct pellet {
	int color; ///<		Player color.\todo Color type.
	int size; ///<		Player size.
	xy_t position; ///<	Player position on the field.
} pellet_t;

///Game field struct.
typedef struct gamefield {
	xy_t size; ///<		Field size.
	player_t *players; ///<	Players array.
	int players_count; ///<	Number of players.
	pellet_t *pellets; ///<	Pellets array.
	int pellets_count; ///<	Number of pellets.
} gamefield_t;

///Game event struct.
typedef struct event {
	int event_id; ///<	Predefined id of event.
	void *arg; ///<		Argument of event.
} event_t;

/**Create game field.
\param int		Field width.
\param int		Field heigth.
\return gamefield_t*	Returns pointer to struct gamefield.
*/
gamefield_t* gamefield_create(int, int);

/**Add player to game field.
\param gamefield_t*	Gamefield struct pointer.
\return int		Returns number of player if success, else -1.
*/
int gamefield_add(gamefield_t*);

/**Begin server calculations, collision detections etc.
\param gamefield_t*	Gamefield struct pointer.
\param event_t* 	Array of events pointer.\todo Maybe implement socket. But i hope there will be ony one elment.
\return int		Return 0 of success, else -1;	
*/
int gamefield_start(gamefield_t*, event_t*);
#endif
