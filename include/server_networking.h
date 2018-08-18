#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef SERVER_NETWORKING_H
#define SERVER_NETWORKING_H

///Struct for storage of player socked file descriptor and address.
typedef struct player_sockaddr {
	int sfd; ///<				Socket ID player binded to.
	struct sockaddr_in sa; ///<	Player sockaddr.
} player_sockaddr_t;

/**Create listener socket.
\param gamefield	Gamefield struct pointer.
\param event_out	List of output events pointer.
\param event_in		List of input events pointer.
\return int		Socket ID.	
*/
int networking_start(gamefield_t *gamefield, event_t **event_out, event_t **event_in);

/**Cancel server networking threads, free send/recieve data pointers.
\return int		Return 0 if success, else spinning emmensly begging for death.
*/
int networking_free();

/**Add players when connecting cycle.
\param v_gamefield	Gamefield struct pointer.
*/
void *cycle_add_player(void* v_gamefield);

/**Listen for incoming events.
\param v_gamefield	Gamefield struct pointer. Used when sync needed.
*/
void *cycle_listen_events(void *v_gamefield);

/**Read event_in list and change player moving variables cycle.
*/
void *cycle_send_events();
#endif
