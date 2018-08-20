#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef CLIENT_NETWORKING_H
#define CLIENT_NETWORKING_H

/**Connect to game server.
\return int		Player ID.	
*/
int connect_to_server();

/**Listen for event.
\return	Event or event_t with event_id = -1 if no events found.
*/
event_t listen_event();

/**Send event.
\param event	Event to send.
\return	int		0 if success, -1 otherwise.
*/
int send_event(event_t event);
#endif
