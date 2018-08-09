#include <pthread.h>
#include <malloc.h>

#ifndef SERVER_CONTROLS_H
#define SERVER_CONTROLS_H

///Struct for two values X, Y.
struct xy {
	int x; ///<	X value.
	int y; ///<	Y value.
};

///Struct containing global game parameters.
struct game_parameters {
	int def_size; ///<		Default player size.
	float def_speed; ///<	Default player speed.
	float def_drag; ///<	Default drag force.
};

///Struct containing player parameters.
struct player {
	int color; ///<				Player color.//TODO color type 
	int size; ///<				Player size.
	struct xy position; ///<	Player position on the field.
	struct xy speed; ///<		Player speed in X, Y direction.
	float drag; ///<			How much force slows player down.
};

///Game field struct.
struct gamefield {
	struct xy size; ///<			Field size.
	struct player *players; ///<	Players array.
	int players_count; ///<			Number of players.
};

/**Create game field.
\param int					Field width.
\param int					Field heigth.
\return struct gamefield*	Returns pointer to struct gamefield.
*/
struct gamefield* gamefield_create(int, int);

/**Add player to game field.
\param struct gamefield*	Gamefield struct pointer.
\return int					Returns number of player if success, else -1.
*/
int gamefield_add(struct gamefield*);

/**Begin server calculations, collision detections etc.
\param struct gamefield*	Gamefield struct pointer.
\return int					Return 0 of success, else -1;	
*/
int gamefield_start(struct gamefield*);
#endif
