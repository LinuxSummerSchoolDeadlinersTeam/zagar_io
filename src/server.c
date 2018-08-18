#include "../include/server_controls.h"
#include "../include/server_networking.h"
#include "../include/client_networking.h"

int main()
{
	gamefield_t *gamefield = gamefield_create();
	event_t *event_in = NULL;
	event_t *event_out = NULL;
	gamefield_start(gamefield, &event_out, &event_in);
	networking_start(gamefield, &event_out, &event_in);
	while(getchar() != 'q') {}
	networking_free();
	gamefield_free(gamefield);
	return 0;
}