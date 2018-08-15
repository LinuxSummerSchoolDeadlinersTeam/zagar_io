#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#ifndef SERVER_CONTROLS_H
#define SERVER_CONTROLS_H

///Event id when player eaten by another.\warning Need both args when used in event_t! x - player eaten id and y - player ate id.
#define EVENT_PLAYER_EATEN 1
///Event id when player ate a pellet.\warning Need both args when used in event_t! x - pellet eaten id and y - pellet ate id.
#define EVENT_PELLET_EATEN 2
///Event id when player change size.\warning Need both args when used in event_t! x - player id and y - size.
#define EVENT_PLAYER_SIZE 3
///Event id when player moves.\warning Need both args when used in event_t! x - player id and y = 1 - up, 2 - down, 3 - left, 4 - right and 0 - if all buttons released.
#define EVENT_PLAYER_MOVE 4
///Event id when syncronization needed.\warning No arguments needed.
#define EVENT_SYNC 5

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
	int field_width; ///<		Gamefield width.
	int field_height; ///<		Gamefield height.
	int field_size_mul; ///<	Game field multiplier.
} game_parameters_t;

///Struct containing player parameters.
typedef struct player {
	int id; ///<		Player id.
	int alive; ///<		Player status. 1 - alive, 0 - dead.
	int color; ///<		Player color.
	int size; ///<		Player size.
	xy_t position; ///<	Player position on the field.
	xy_t speed; ///<	Player speed in X, Y direction.
	xy_t accel; ///<	Player acceleration in X, Y direction.
	int drag; ///<		How much force slows player down.
} player_t;

///Struct containing player parameters.
typedef struct pellet {
	int color; ///<		Pellet color.
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
\return gamefield_t*	Returns pointer to struct gamefield based on game.cfg file.
*/
gamefield_t* gamefield_create();

/**Add player to game field.
\param gamefield	Gamefield struct pointer.
\return int		Returns number of player if success, else -1.
*/
int gamefield_add(gamefield_t* gamefield);

/**Begin server calculations, collision detections etc.
\param gamefield	Gamefield struct pointer.
\param event_out	List of output events pointer.
\param event_in		List of input events pointer.
\return int		Return 0 if success, else -1.	
*/
int gamefield_start(gamefield_t* gamefield, event_t** event_out, event_t** event_in);

/**Cancel server controls threads, free gamefield structures.
\param gamefield	Gamefield struct pointer.
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

/**Make pellets cycle.
\param v_gamefield	Gamefield struct pointer.
*/
void *cycle_make_pellet(void* v_gamefield);

/**Move players cycle.
\param v_gamefield	Gamefield struct pointer.
*/
void *cycle_move(void* v_gamefield);

/**Read event_in list and change player moving variables cycle.
\param v_gamefield	Gamefield struct pointer.
*/
void *cycle_controls_in(void* v_gamefield);

/**Detect collision and add events to event_out cycle.
\param v_gamefield	Gamefield struct pointer.
*/
void *cycle_controls_out(void* v_gamefield);
#endif
