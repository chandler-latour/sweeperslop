//This file defines a gamestate datastructure and some functions around it
//a gamestate data structure is a gamestate that must be started, that has a function to update per 1 frame based on deltatime since the last frame, and a function to clean it up, including all allocated memory. 
//These data structures will also point to a "gamedata" object in memory, which can be very vague.
#ifndef GAMESTATE
#define GAMESTATE
#include <assert.h>

typedef struct s_gamestate {
	int (*f_update)(void *); //returns a return code based on the outcome of a loop
	//these vary between function types, with the sole exception that 0 always means "continue"
	//("business as usual, keep going")
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

/*
 * enums for a bit array. Each bit here is a boolean for a certain property. 
 */
enum LINKED_GS_FLAGS { 
	UPDATE=1,
	DRAW=2
};

//a linked list for the gamestates
//means we can go thru each gamestate and update them one at a time
typedef struct s_linked_gamestate {
	t_gamestate * gs;
	t_gamestate * nexts;
	void (*f_handle)(int, struct s_linked_gamestate *); //a function for handling the return codes from f_update
	//it has to be in s_linked_gamestate and not s_gamestate so that it can
	//create new linked gamestates and link them. This is because s_linked_gamestate relies on s_gamestate, and
	//it isn't defined within s_gamestate's scope up in the source file. The only other way to get around that
	//snag would be to work with a void pointer. I think void pointers are spooky and I try to avoid them
	//if it is possible and handy to do so. I'm in a situation where I know exactly WHAT TYPE of data needs to
	//go in the function, I don't need flexibility. 
	unsigned int flags;
} t_linked_gamestate;


void IterateLinkedGamestates(t_linked_gamestate *);

#endif
