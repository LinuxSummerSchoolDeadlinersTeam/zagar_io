#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifndef SERVER_CONTROLS_H
#define SERVER_CONTROLS_H

///Event id when player eaten by another.\warning Need both args when used in event_t! x - player eaten id and y - player ate id.
#define EVENT_PLAYER_EATEN 1
///Event id when player ate a pellet.\warning Need both args when used in event_t! x - pellet eaten id and y - pellet ate id.
#define EVENT_PELLET_EATEN 2
///Event id when player change size.\warning Need both args when used in event_t! x - player id and y - size.\todo Shoot yourself into players.
#define EVENT_PLAYER_SIZE 3
///Event id when player moves.\warning Need only argument X when used! 1 - up, 2 - down, 3 - left, 4 - right and 0 - if all buttons released.
#define EVENT_PLAYER_MOVE 4

///Struct for two values type of int.\note Used for coordinates, speed, or as argument for event.
typedef struct xy {
	int x; ///<	First value.
	int y; ///<	Second value.
} xy_t;

///Struct containing global game parameters.
typedef struct game_parameters {
	int def_player_size; ///<	Default player size.
	int def_pellet_size; ///<	Default pellet size.
	int def_speed; ///<		Default maximum player speed.
	int def_accel; ///<		Default acceleration.
	int def_drag; ///<		Default drag force.
	useconds_t player_pace; ///<	Time in usec between every move cycle.
	useconds_t pellet_pace; ///<	Time in usec between every pellet generation.
} game_parameters_t;

///Struct containing player parameters.
typedef struct player {
	int color; ///<		Player color.\todo Color type.
	int size; ///<		Player size.
	xy_t position; ///<	Player position on the field.
	xy_t speed; ///<	Player speed in X, Y direction.
	xy_t accel; ///<	Player acceleration in X, Y direction.
	int drag; ///<		How much force slows player down.
} player_t;

///Struct containing player parameters.
typedef struct pellet {
	int color; ///<		Pellet color.\todo Color type.
	int size; ///<		Pellet size.
	xy_t position; ///<	Pellet position on the field.
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
	int arg_x; ///<		Argument X of event.
	int arg_y; ///<		Argument Y of event.
	struct event *next; ///<Pointer to next event (NULL by default).
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
\param event_t** 	List of output events pointer.\todo Maybe implement socket. But i hope there will be ony one elment.
\param event_t** 	List of input events pointer.\todo Maybe implement socket. But i hope there will be ony one elment.
\return int		Return 0 if success, else -1.	
*/
int gamefield_start(gamefield_t*, event_t**, event_t**);

/**Cancel server controls threads, free gamefield structures.\warning Won't free events arrays.
\param gamefield_t*	Gamefield struct pointer.
\return int		Return 0 if success, else spinning emmensly begging for death.
*/
int gamefield_free(gamefield_t* gamefield);

/**Insert event into event list.
\param event_list	Address of (pointer to) event list pointer.\warning Pointer must be NULL before use. For example: event_t *event_in = NULL; event_set(&event_in, event);
\param event		Event to insert.
\return int		Return 0 if success, else -1.
*/
int event_set(event_t** event_list, event_t event);

/**Get event from event list.
\param event_list	Address of (pointer to) event list pointer.
\return event_t		Return last event from list, or event_t with event_id = 0 if no events found.
*/
event_t event_get(event_t** event_list);
#endif
