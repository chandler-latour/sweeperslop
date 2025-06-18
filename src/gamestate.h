//This file defines a gamestate datastructure and some functions around it
//a gamestate data structure is a gamestate that must be started, that has a function to update per 1 frame based on deltatime since the last frame, and a function to clean it up, including all allocated memory. 
//These data structures will also point to a "gamedata" object in memory, which can be very vague.
#ifndef GAMESTATE
#define GAMESTATE

typedef struct s_gamestate {
	void (*f_init)();
	void (*f_update)(void *);
	void (*f_draw)();
	void (*f_close)();
	void * game_data;
} t_gamestate;

void f_pass(); //empty function, literally does nothing, use this for
//gamestates that don't need an init function


#endif
