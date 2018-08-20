#include "../include/server_controls.h"
#include "../include/server_networking.h"
#include "../include/client_networking.h"

#define SERVER_NETWORKING_THREADS_COUNT 3

player_sockaddr_t sfd_player;

int connect_to_server()
{
	struct sockaddr_in sockaddr;
	int event = 5;
	int player_id;
	sfd_player.sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(3110);
	inet_aton("127.0.0.1", &sockaddr.sin_addr);

	socklen_t addrlen = sizeof(sockaddr);
	sendto(sfd_player.sfd, &event, sizeof(event), 0, (struct sockaddr*)&sockaddr, addrlen);
	recvfrom(sfd_player.sfd, &player_id, sizeof(int), 0, (struct sockaddr*)&sockaddr, &addrlen);
	sfd_player.sa = sockaddr;
	return player_id;
}

event_t listen_event()
{
	event_t *event_ptr = malloc(sizeof(event_t));
	event_t event;
	socklen_t addrlen = sizeof(sfd_player.sa);
	if(recvfrom(sfd_player.sfd, event_ptr, sizeof(event_t), MSG_DONTWAIT, (struct sockaddr*)&sfd_player.sa, &addrlen) != -1)
	{
		event = *event_ptr;
	}
	else
		event.event_id = -1;
	return event;
}

int send_event(event_t event)
{
	if(sendto(sfd_player.sfd, &event, sizeof(event), 0, (struct sockaddr*)&sfd_player.sa, sizeof(sfd_player.sa)) == -1)
	{
		perror("send_event: Can't sent event");
		return -1;
	}
	else
		return 0;
}