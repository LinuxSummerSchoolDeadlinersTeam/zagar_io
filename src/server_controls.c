#include "../include/server_controls.h"

#define SERVER_CONTROLS_THREADS_COUNT 4
game_parameters_t game_parameters;
pthread_t *tid;
event_t **event_glob_in;
event_t **event_glob_out;
pthread_spinlock_t spinlock_event;

int gamefield_free(gamefield_t* gamefield)
{
	int i;
	//Cancel threads
	for(i = 0; i < SERVER_CONTROLS_THREADS_COUNT; i++)
	{
		pthread_cancel(tid[i]);
		pthread_join(tid[i], NULL);
	}
	free(tid);
	//Free events lists
	if(*event_glob_in != NULL)
	{
		event_t *event_ptr;
		event_ptr = *event_glob_in;
		if(event_ptr->next == NULL)
			free(event_ptr);
		else
		{
			event_t *event_prev_ptr;
			while(event_ptr->next != NULL)
			{
				event_prev_ptr = event_ptr;
				event_ptr = event_ptr->next;
				free(event_prev_ptr);
			}
			free(event_ptr);
		}
	}
	if(*event_glob_out != NULL)
	{
		event_t *event_ptr;
		event_ptr = *event_glob_out;
		if(event_ptr->next == NULL)
			free(event_ptr);
		else
		{
			event_t *event_prev_ptr;
			while(event_ptr->next != NULL)
			{
				event_prev_ptr = event_ptr;
				event_ptr = event_ptr->next;
				free(event_prev_ptr);
			}
			free(event_ptr);
		}
	}
	//Free gamefield
	free(gamefield->players);
	free(gamefield->pellets);
	free(gamefield);
	pthread_spin_destroy(&spinlock_event);
	return 0;
}

gamefield_t* gamefield_create(int width, int height)
{
	srand(time(NULL));
	pthread_spin_init(&spinlock_event, PTHREAD_PROCESS_SHARED);
	int option_count = 0;
	char *buf = malloc(1024);
	int value;
	FILE *file = fopen("../game.cfg", "r");
	if(file == NULL)
		perror("Can't open file");
	else
		while(fgets(buf, 1024, file) != NULL) {
			if(buf[0] != '#') {
				value = atoi(buf);
				switch(option_count) {
					case 0: game_parameters.def_player_size = value; break;
					case 1: game_parameters.def_pellet_size = value; break;
					case 2: game_parameters.def_speed = value; break;
					case 3: game_parameters.def_accel = value; break;
					case 4: game_parameters.def_drag = value; break;
					case 5: game_parameters.player_pace = (useconds_t)value; break;
					case 6: game_parameters.pellet_pace = (useconds_t)value; break;
					default: break;
				}
				option_count++;
			}
		}
	fclose(file);
	free(buf);
	gamefield_t *gamefield = malloc(sizeof(gamefield_t));
	xy_t size = {width, height};
	gamefield->size = size;
	gamefield->players = malloc(0);
	gamefield->players_count = 0;
	gamefield->pellets = malloc(0);
	gamefield->pellets_count = 0;
	return gamefield;
}

int gamefield_add(gamefield_t* gamefield)
{
	int player_id = gamefield->players_count;
	gamefield->players = realloc(gamefield->players, (gamefield->players_count + 1) * sizeof(player_t));
	if(gamefield->players == NULL) return -1;
	player_t player;
	player.alive = 1;
	player.color = gamefield->players_count + 1; //color equal player id
	player.size = game_parameters.def_player_size;
	player.position.x = rand()%gamefield->size.x;
	player.position.y = rand()%gamefield->size.y;
	player.speed.x = 0;
	player.speed.y = 0;
	player.accel.x = 0;
	player.accel.y = 0;
	player.drag = game_parameters.def_drag;
	gamefield->players[player_id] = player;
	gamefield->players_count++;
	return player_id;
}

void *cycle_make_pellet(void* v_gamefield)
{
	gamefield_t *gamefield = v_gamefield;
	while(1)
	{
		gamefield->pellets = realloc(gamefield->pellets, (gamefield->pellets_count + 1) * sizeof(pellet_t));
		pellet_t pellet;
		pellet.color = 0;
		pellet.size = game_parameters.def_pellet_size;
		pellet.position.x = rand()%gamefield->size.x;
		pellet.position.y = rand()%gamefield->size.y;
		gamefield->pellets[gamefield->pellets_count] = pellet;
		gamefield->pellets_count++;
		usleep(game_parameters.pellet_pace);
		pthread_testcancel();
	}
}

void *cycle_move(void* v_gamefield)
{
	int i;
	gamefield_t *gamefield = v_gamefield;
	while(1)
	{
		for(i = 0; i < gamefield->players_count; i++)
		{
			if(gamefield->players[i].alive)
			{
				//Acceleration
				//horizontal
				if(abs(gamefield->players[i].speed.x) < game_parameters.def_speed)
					gamefield->players[i].speed.x += gamefield->players[i].accel.x;
				else
					if(gamefield->players[i].speed.x >= 0)
						gamefield->players[i].speed.x = game_parameters.def_speed;
					else
						gamefield->players[i].speed.x = -1 * game_parameters.def_speed;
				//vertical
				if(abs(gamefield->players[i].speed.y) < game_parameters.def_speed)
					gamefield->players[i].speed.y += gamefield->players[i].accel.y;
				else
					if(gamefield->players[i].speed.y >= 0)
						gamefield->players[i].speed.y = game_parameters.def_speed;
					else
						gamefield->players[i].speed.y = -1 * game_parameters.def_speed;
				//Drag
				//horizontal
				if(gamefield->players[i].speed.x > 0)
					if(gamefield->players[i].speed.x - gamefield->players[i].drag > 0)
						gamefield->players[i].speed.x -= gamefield->players[i].drag;
					else
						gamefield->players[i].speed.x = 0;
				if(gamefield->players[i].speed.x < 0)
					if(gamefield->players[i].speed.x + gamefield->players[i].drag < 0)
						gamefield->players[i].speed.x += gamefield->players[i].drag;
					else
						gamefield->players[i].speed.x = 0;
				//vertical
				if(gamefield->players[i].speed.y > 0)
					if(gamefield->players[i].speed.y - gamefield->players[i].drag > 0)
						gamefield->players[i].speed.y -= gamefield->players[i].drag;
					else
						gamefield->players[i].speed.y = 0;
				if(gamefield->players[i].speed.y < 0)
					if(gamefield->players[i].speed.y + gamefield->players[i].drag < 0)
						gamefield->players[i].speed.y += gamefield->players[i].drag;
					else
						gamefield->players[i].speed.x = 0;
				//Move
				gamefield->players[i].position.x += gamefield->players[i].speed.x;
				gamefield->players[i].position.y += gamefield->players[i].speed.y;
				//Wall hit
				//right
				if(gamefield->players[i].position.x >= gamefield->size.x) {
					gamefield->players[i].position.x = gamefield->size.x;
					gamefield->players[i].accel.x = 0;
					gamefield->players[i].speed.x = 0;
					continue;
				}
				//left
				if(gamefield->players[i].position.x <= 0) {
					gamefield->players[i].position.x = 0;
					gamefield->players[i].accel.x = 0;
					gamefield->players[i].speed.x = 0;
					continue;
				}
				//bottom
				if(gamefield->players[i].position.y >= gamefield->size.y) {
					gamefield->players[i].position.y = gamefield->size.y;
					gamefield->players[i].accel.y = 0;
					gamefield->players[i].speed.y = 0;
					continue;
				}
				//top
				if(gamefield->players[i].position.y <= 0) {
					gamefield->players[i].position.y = 0;
					gamefield->players[i].accel.y = 0;
					gamefield->players[i].speed.y = 0;
					continue;
				}
			}
		}
		usleep(game_parameters.player_pace);
		pthread_testcancel();
	}
}

//struct gamefield_and_events {
//	gamefield_t *gamefield;
//	event_t ***event;
//};

//void *cycle_controls_in(void* v_gamefield_and_events)
void *cycle_controls_in(void* v_gamefield)
{
	//struct gamefield_and_events *gamefield_and_events = v_gamefield_and_events;
	//gamefield_t *gamefield = gamefield_and_events->gamefield;
	//event_t **event_in = *gamefield_and_events->event;
	gamefield_t *gamefield = v_gamefield;
	event_t event;
	while(1)
	{
		//event = event_get(event_in);
		event = event_get(event_glob_in);
		if(event.event_id != -1) //if event got
			if(gamefield->players[event.arg_x].alive) //if palyer alive
				switch(event.event_id) {
					case 4:
						switch(event.arg_y) {
							case 1:
								gamefield->players[event.arg_x].accel.y = -1 * game_parameters.def_accel;
								gamefield->players[event.arg_x].accel.x = 0;
								break;
							case 2:
								gamefield->players[event.arg_x].accel.y = game_parameters.def_accel;
								gamefield->players[event.arg_x].accel.x = 0;
								break;
							case 3:
								gamefield->players[event.arg_x].accel.x = -1 * game_parameters.def_accel;
								gamefield->players[event.arg_x].accel.y = 0;
								break;
							case 4:
								gamefield->players[event.arg_x].accel.x = game_parameters.def_accel;
								gamefield->players[event.arg_x].accel.y = 0;
								break;
							case 0:
								gamefield->players[event.arg_x].accel.y = 0;
								gamefield->players[event.arg_x].accel.x = 0;
								break;
						}
						break;
				}
		pthread_testcancel();
	}
}

//void *cycle_controls_out(void* v_gamefield_and_events)
void *cycle_controls_out(void* v_gamefield)
{
	//struct gamefield_and_events *gamefield_and_events = v_gamefield_and_events;
	//gamefield_t *gamefield = gamefield_and_events->gamefield;
	//event_t **event_out = *gamefield_and_events->event;
	gamefield_t *gamefield = v_gamefield;
	//int i=0;
	event_t event;
	int comp_who, comp_with, pellet_i;
	float distance;
	while(1)
	{
		//i++;
		//event.event_id = i;
		//event.arg_x=i;
		//event.arg_y =i;
		//event.next=NULL;
		//if(event_set(event_out, event) < 0)
	 	//if(event_set(event_glob_out, event) < 0)
		//	perror("Can't set event");
		//sleep(1);
		event.next = NULL;
		event.event_id = EVENT_PLAYER_EATEN;
		//Player collision detection
		if(gamefield->players_count > 1)
			for(comp_who = 0; comp_who < gamefield->players_count - 1; comp_who++)
				for(comp_with = comp_who + 1; comp_with < gamefield->players_count; comp_with++)
				{
					if(gamefield->players[comp_who].alive && gamefield->players[comp_with].alive) //if both players alive
					{
						distance = sqrt( //hypotesis
							pow(abs(gamefield->players[comp_who].position.x - gamefield->players[comp_with].position.x), 2) + //leg x
							pow(abs(gamefield->players[comp_who].position.y - gamefield->players[comp_with].position.y), 2)); //leg y

						//if(comp_who == 0 && comp_with == 1)
						//{
						//	printf("DIST: %f\n", distance);
						//}
						//Player eaten event
						if(distance <= gamefield->players[comp_who].size)
						{
							gamefield->players[comp_with].alive = 0;
							event.arg_x = comp_who;
							event.arg_y = comp_with;
							if(event_set(event_glob_out, event) < 0)
								perror("Can't set event");
							continue;
						}
						if(distance <= gamefield->players[comp_with].size)
						{
							gamefield->players[comp_who].alive = 0;
							event.arg_x = comp_with;
							event.arg_y = comp_who;
							if(event_set(event_glob_out, event) < 0)
								perror("Can't set event");
							continue;
						}
					}
				}
		//Pellet collision detection

		if(gamefield->players_count > 0)
			for(comp_who = 0; comp_who < gamefield->players_count; comp_who++)
			{
				for(comp_with = 0; comp_with < gamefield->pellets_count; comp_with++)
				{
					if(gamefield->players[comp_who].alive) //if player alive
					{
						distance = sqrt( //hypotesis
							pow(abs(gamefield->players[comp_who].position.x - gamefield->pellets[comp_with].position.x), 2) + //leg x
							pow(abs(gamefield->players[comp_who].position.y - gamefield->pellets[comp_with].position.y), 2)); //leg y
						if(distance <= gamefield->players[comp_who].size)
						{
							//Size increase event
							gamefield->players[comp_who].size += gamefield->pellets[comp_with].size;
							event.event_id = EVENT_PLAYER_SIZE;
							event.arg_x = comp_who;
							event.arg_y = gamefield->pellets[comp_with].size;
							if(event_set(event_glob_out, event) < 0)
								perror("Can't set event");
							//printf("PELLET %d: %f\n", comp_with, distance);
							//Remove pellet from array
							for(pellet_i = comp_with; pellet_i < gamefield->pellets_count - 1; pellet_i++)
								gamefield->pellets[pellet_i] = gamefield->pellets[pellet_i+1];
							gamefield->pellets_count--;
							gamefield->pellets = realloc(gamefield->pellets, (gamefield->pellets_count) * sizeof(pellet_t));
							//Pellet eaten event
							event.event_id = EVENT_PELLET_EATEN;
							event.arg_x = comp_who;
							event.arg_y = comp_with;
							if(event_set(event_glob_out, event) < 0)
								perror("Can't set event");
							break;
						}
					}
				}
			}
		pthread_testcancel();
	}
}

int gamefield_start(gamefield_t* gamefield, event_t** event_out, event_t** event_in)
{
	//struct gamefield_and_events gamefield_and_events_in, gamefield_and_events_out;
	tid = malloc(SERVER_CONTROLS_THREADS_COUNT * sizeof(pthread_t));
	event_glob_out = event_out;
	event_glob_in = event_in;
	if(tid == NULL)
		return -1;

	if(pthread_create(&tid[0], NULL, cycle_make_pellet, gamefield) > 0)
		return -1;

	if(pthread_create(&tid[1], NULL, cycle_move, gamefield) > 0)
		return -1;

	//gamefield_and_events_in.gamefield = gamefield;
	//gamefield_and_events_in.event = &event_in;
	//if(pthread_create(&tid[2], NULL, cycle_controls_in, &gamefield_and_events_in) > 0)
	if(pthread_create(&tid[2], NULL, cycle_controls_in, gamefield) > 0)
		return -1;
	//gamefield_and_events_out.gamefield = gamefield;
	//gamefield_and_events_out.event = &event_out;
	//if(pthread_create(&tid[3], NULL, cycle_controls_out, &gamefield_and_events_out) > 0)
	if(pthread_create(&tid[3], NULL, cycle_controls_out, gamefield) > 0)
		return -1;
	return 0;
}

int event_set(event_t **event_list, event_t event)
{
	pthread_spin_lock(&spinlock_event);
	event_t *new_event = malloc(sizeof(event_t));
	if(new_event == NULL)
		return -1;
	*new_event = event;
	if(*event_list == NULL) {
		*event_list = new_event;
		pthread_spin_unlock(&spinlock_event);
		return 0;
	}
	else {
		event_t *event_ptr = *event_list;
		while(event_ptr->next != NULL)
			event_ptr = event_ptr->next;
		event_ptr->next = new_event;
		pthread_spin_unlock(&spinlock_event);
		return 0;
	}
	pthread_spin_unlock(&spinlock_event);
	return -1;
}

event_t event_get(event_t **event_list)
{
	event_t got_event;
	got_event.event_id = -1;
	pthread_spin_lock(&spinlock_event);
	if(*event_list == NULL) {
		pthread_spin_unlock(&spinlock_event);
		return got_event;
	}

	if((*event_list)->next == NULL) {
		got_event = **event_list;
		free(*event_list);
		*event_list = NULL;
		pthread_spin_unlock(&spinlock_event);
		return got_event;
	}
	else {
		event_t *prev_event_ptr;
		event_t *event_ptr = *event_list;
		while(event_ptr->next != NULL) {
			prev_event_ptr = event_ptr;
			event_ptr = event_ptr->next;
		}
		got_event = *event_ptr;
		free(event_ptr);
		prev_event_ptr->next = NULL;
		pthread_spin_unlock(&spinlock_event);
		return got_event;
	}
	pthread_spin_unlock(&spinlock_event);
	return got_event;
}