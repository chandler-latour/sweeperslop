//This file defines a gamestate datastructure and some functions around it
//a gamestate data structure is a gamestate that must be started, that has a function to update per 1 frame based on deltatime since the last frame, and a function to clean it up, including all allocated memory. 
//These data structures will also point to a "gamedata" object in memory, which can be very vague.
#ifndef GAMESTATE
#define GAMESTATE

typedef struct s_gamestate {
	//void (*f_instantiate)(); //the function to instantiate properties into memory
	//void (*f_init)(void *); //the function to get the gamestate ready for use
	/*
	I can't feed arguments to these functions because the structure needs to be
	flexible and accommodate a large variety of different options
	so, I feed it a single void pointer and it could mean anything.
	In practice, that pointer will be a pointer to the game state's
	game data.

	So, setting up a gamestate will go:
	t_gamestate gs;
	(assign the functions)
	(instantiate the game data)
	(assign the game data as desired)
	gs.init(&gs.game_data)
	gs.f_instantiate();
	*/
	void (*f_update)(void *);
	void (*f_draw)(void *, void *);
	void (*f_close)(struct s_gamestate *);
	void * game_data;
	void * graphics_data;
} t_gamestate;

void f_pass(); //empty function, literally does nothing, use this for
//gamestates that don't need an init function

/*
a helper function that runs both the update and draw function of a gamestate in that order. I'm structuring it like this because I want it to be possible to sometimes update a gamestate but not draw it, such as if it's in the background of something else.
*/
void comboloop(void *);


//a linked list for the gamestates
//means we can go thru each gamestate and update them one at a time
typedef struct s_linked_gamestate {
	t_gamestate * gs;
	t_gamestate * next_gs;
} t_linked_gamestate;

#endif
