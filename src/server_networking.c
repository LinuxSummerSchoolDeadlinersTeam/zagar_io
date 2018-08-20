#include "../include/server_controls.h"
#include "../include/server_networking.h"

#define SERVER_NETWORKING_THREADS_COUNT 3

int sfd_listener;
player_sockaddr_t *sfd_players;
int sfd_players_count;
pthread_t *tid_networking;
event_t *event_add;
event_t *event_listen;
event_t **event_glob_networking_in;
event_t **event_glob_networking_out;
pthread_spinlock_t spinlock_players;

int networking_free()
{
	int i;
	free(sfd_players);
	//Cancel threads
	for(i = 0; i < SERVER_NETWORKING_THREADS_COUNT; i++)
	{
		pthread_cancel(tid_networking[i]);
		pthread_join(tid_networking[i], NULL);
	}
	free(tid_networking);
	free(event_add);
	free(event_listen);
	pthread_spin_destroy(&spinlock_players);
	return 0;
}
void *cycle_add_player(void* v_gamefield)
{
	struct sockaddr_in sockaddr;
	gamefield_t *gamefield = v_gamefield;
	socklen_t addrlen;
	event_add = malloc(sizeof(event_t));
	int player_id;
	while(1)
	{
		addrlen = sizeof(sfd_players[player_id].sa);
		recvfrom(sfd_listener, event_add, sizeof(event_t), 0, (struct sockaddr*)&sockaddr, &addrlen);
		if(event_add->event_id == EVENT_SYNC)
		{
			pthread_spin_lock(&spinlock_players);
			player_id = gamefield_add(gamefield);
			sfd_players_count = gamefield->players_count;
			sfd_players = realloc(sfd_players, sfd_players_count * sizeof(player_sockaddr_t));
			pthread_spin_unlock(&spinlock_players);

			sfd_players[player_id].sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			sfd_players[player_id].sa = sockaddr;

			sockaddr.sin_family = AF_INET;
			sockaddr.sin_port = 0;
			inet_aton("127.0.0.1", &sockaddr.sin_addr);

			bind(sfd_players[player_id].sfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

			sendto(sfd_players[player_id].sfd, &player_id, sizeof(int), 0, (struct sockaddr*)&sfd_players[player_id].sa, sizeof(sfd_players[player_id].sa));
		}
		pthread_testcancel();
	}
}

void *cycle_listen_events(void *v_gamefield)
{
	gamefield_t *gamefield = v_gamefield;
	event_listen = malloc(sizeof(event_t));
	event_t event;
	socklen_t addrlen;
	int i;
	while(1)
	{
		for(i=0; i<sfd_players_count; i++)
		{
			addrlen = sizeof(sfd_players[i].sa);
			if(recvfrom(sfd_players[i].sfd, event_listen, sizeof(event_t), MSG_DONTWAIT, (struct sockaddr*)&sfd_players[i].sa, &addrlen) != -1)
			{
				event = *event_listen;
				if(event.event_id == EVENT_SYNC)
				{
					gamefield_t gamefield_to_send = *gamefield;
					sendto(sfd_players[event.arg_x].sfd, &gamefield_to_send, sizeof(gamefield_to_send), 0, (struct sockaddr*)&sfd_players[event.arg_x].sa, sizeof(sfd_players[event.arg_x].sa));
				}
				if(event_set(event_glob_networking_in, event) < 0)
					perror("Can't set event");
			}
		}
		pthread_testcancel();
	}
}

void *cycle_send_events()
{
	event_t event;
	int i;
	while(1)
	{
		event = event_get(event_glob_networking_out);
		if(event.event_id != -1)
		{
			for(i=0; i<sfd_players_count; i++)
			{
				sendto(sfd_players[i].sfd, &event, sizeof(event), 0, (struct sockaddr*)&sfd_players[i].sa, sizeof(sfd_players[i].sa));
			}
		}
		pthread_testcancel();
	}
}

int networking_start(gamefield_t *gamefield, event_t **event_out, event_t **event_in)
{
	pthread_spin_init(&spinlock_players, PTHREAD_PROCESS_SHARED);
	event_glob_networking_out = event_out;
	event_glob_networking_in = event_in;
	sfd_players = malloc(0);
	struct sockaddr_in sockaddr;
	sfd_listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(3110);
	inet_aton("127.0.0.1", &sockaddr.sin_addr);
	bind(sfd_listener, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

	tid_networking = malloc(SERVER_NETWORKING_THREADS_COUNT * sizeof(pthread_t));
	if(tid_networking == NULL)
		return -1;

	if(pthread_create(&tid_networking[0], NULL, cycle_add_player, gamefield) > 0)
		return -1;

	if(pthread_create(&tid_networking[1], NULL, cycle_listen_events, gamefield) > 0)
		return -1;

	if(pthread_create(&tid_networking[2], NULL, cycle_send_events, NULL) > 0)
		return -1;
	return 0;
}
