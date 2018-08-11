#include "../include/server_controls.h"

#define SERVER_CONTROLS_THREADS_COUNT 4
game_parameters_t game_parameters;
pthread_t *tid;
event_t **event_glob_in;
event_t **event_glob_out;
//pthread_spinlock_t spinlock;
pthread_mutex_t spinlock;///<\todo EBANYE SPINLOKI

int gamefield_free(gamefield_t* gamefield)
{
	int i;
	for(i = 0; i < SERVER_CONTROLS_THREADS_COUNT; i++)
	{
		pthread_cancel(tid[i]);
		pthread_join(tid[i], NULL);
	}
	free(tid);
	free(gamefield->players);
	free(gamefield->pellets);
	free(gamefield);
	//pthread_spin_destroy(&spinlock);
	pthread_mutex_destroy(&spinlock);
	return 0;
}

gamefield_t* gamefield_create(int width, int height)
{
	srand(time(NULL));
	//pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&spinlock, NULL);
	game_parameters.def_player_size = 10;
	game_parameters.def_pellet_size = 5;
	game_parameters.def_speed = 100;
	game_parameters.def_accel = 10;
	game_parameters.def_drag = 1;
	game_parameters.player_pace = 200000;
	game_parameters.pellet_pace = 1000000;

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
	player.color = 0; ///\todo Random color.
	player.size = game_parameters.def_player_size;
	player.position.x = rand()%gamefield->size.x;
	player.position.y = rand()%gamefield->size.y;
	player.speed.x = 0;
	player.speed.y = 0;
	player.accel.x = 0;
	player.accel.y = 0;
	player.drag = 0;
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
		pellet.color = 0; ///\todo Color.
		pellet.size = game_parameters.def_pellet_size;
		pellet.position.x = rand()%gamefield->size.x;
		pellet.position.y = rand()%gamefield->size.y;
		gamefield->pellets[gamefield->pellets_count] = pellet;
		gamefield->pellets_count++;
		usleep(game_parameters.pellet_pace); ///\todo Global timer.
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
			//Acceleration
			if(gamefield->players[i].speed.x < game_parameters.def_speed)
				gamefield->players[i].speed.x += gamefield->players[i].accel.x;
			else
				gamefield->players[i].speed.x = game_parameters.def_speed;
			if(gamefield->players[i].speed.y < game_parameters.def_speed)
				gamefield->players[i].speed.y += gamefield->players[i].accel.y;
			else
				gamefield->players[i].speed.y = game_parameters.def_speed;
			//Drag
			if(gamefield->players[i].speed.x > 0)
				gamefield->players[i].speed.x -= gamefield->players[i].drag;
			else
				gamefield->players[i].speed.x = 0;
			if(gamefield->players[i].speed.y > 0)
				gamefield->players[i].speed.y -= gamefield->players[i].drag;
			else
				gamefield->players[i].speed.y = 0;
			//Move
			gamefield->players[i].position.x += gamefield->players[i].speed.x;
			gamefield->players[i].position.y += gamefield->players[i].speed.y;
		}
		usleep(game_parameters.player_pace); ///\todo Global timer.
		pthread_testcancel();
	}
}

//struct gamefield_and_events {
//	gamefield_t *gamefield;
//	event_t ***event;
//};

//void *cycle_controls_in(void* v_gamefield_and_events)
void *cycle_controls_in(void* v_gamefield)///<\todo PEREDAT YKAZATEL NA *EVENT V PARAMETER
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
		//if(event.event_id != -1)
		//	printf("!!!!!!!!!!!!!!!!!GOT!!!!!!!!!!!!!!!!!!!!!!!!!!!!%d\n", event.event_id);
		pthread_testcancel();
	}
}

//void *cycle_controls_out(void* v_gamefield_and_events)
void *cycle_controls_out(void* v_gamefield)///<\todo PEREDAT YKAZATEL NA *EVENT V PARAMETER
{
	//struct gamefield_and_events *gamefield_and_events = v_gamefield_and_events;
	//gamefield_t *gamefield = gamefield_and_events->gamefield;
	//event_t **event_out = *gamefield_and_events->event;
	gamefield_t *gamefield = v_gamefield;
	//int i=0;
	//event_t event;
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
	//pthread_spin_lock(&spinlock);
	pthread_mutex_lock(&spinlock);
	event_t *new_event = malloc(sizeof(event_t));
	if(new_event == NULL)
		return -1;
	*new_event = event;
	if(*event_list == NULL) {
		*event_list = new_event;
		//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
		return 0;
	}
	else {
		event_t *event_ptr = *event_list;
		while(event_ptr->next != NULL)
			event_ptr = event_ptr->next;
		event_ptr->next = new_event;
		//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
		return 0;
	}
	//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
	return -1;
}

event_t event_get(event_t **event_list)
{
	event_t got_event;
	got_event.event_id = -1;
	//pthread_spin_lock(&spinlock);
	pthread_mutex_lock(&spinlock);
	if(*event_list == NULL) {
		//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
		return got_event;
	}

	if((*event_list)->next == NULL) {
		got_event = **event_list;
		free(*event_list);
		*event_list = NULL;
		//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
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
		//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
		return got_event;
	}
	//pthread_spin_unlock(&spinlock);
		pthread_mutex_unlock(&spinlock);
	return got_event;
}