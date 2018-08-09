#include <pthread.h>
#include <malloc.h>

#ifndef SERVER_CONTROLS_H
#define SERVER_CONTROLS_H

///Struct for two values X, Y.
typedef struct xy {
	int x; ///<	X value.
	int y; ///<	Y value.
} xy_t;

///Struct containing global game parameters.
typedef struct game_parameters {
	int def_size; ///<		Default player size.
	float def_speed; ///<	Default player speed.
	float def_drag; ///<	Default drag force.
} game_parameters_t;

///Struct containing player parameters.
typedef struct player {
	int color; ///<				Player color.\TODO color type 
	int size; ///<				Player size.
	xy_t position; ///<	Player position on the field.
	xy_t speed; ///<		Player speed in X, Y direction.
	float drag; ///<			How much force slows player down.
} player_t;

///Struct containing player parameters.
typedef struct pellet {
	int color; ///<				Player color.\TODO color type 
	int size; ///<				Player size.
	xy_t position; ///<	Player position on the field.
} pellet_t;

///Game field struct.
typedef struct gamefield {
	xy_t size; ///<			Field size.
	player_t *players; ///<	Players array.
	int players_count; ///<	Number of players.
	pellet_t *pellets; ///<	Pellets array.
	int pellets_count; ///<	Number of pellets.
} gamefield_t;

/**Create game field.
\param int				Field width.
\param int				Field heigth.
\return gamefield_t*	Returns pointer to struct gamefield.
*/
gamefield_t* gamefield_create(int, int);

/**Add player to game field.
\param gamefield_t*	Gamefield struct pointer.
\return int			Returns number of player if success, else -1.
*/
int gamefield_add(gamefield_t*);

/**Begin server calculations, collision detections etc.
\param gamefield_t*	Gamefield struct pointer.
\return int			Return 0 of success, else -1;	
*/
int gamefield_start(gamefield_t*);
#endif
