#include "gamestate.h"


void f_pass() {
	//lmao this function does nothing
}

//helper function to take a gamestate and run the update and draw functions
//for technical reasons it HAS to be a void pointer, but MAKE SURE IT POINTS TO A GAMESTATE!!!!
//BAAAD shit will go down if you point it to something that is NOT A GAMESTATE POINTER
//TODO: implement a way to disable drawing for some gamestates
void comboloop(void * gsv) {
	t_gamestate * gs = (t_gamestate *) gsv;
	gs->f_update(gs->game_data);
	gs->f_draw(gs->game_data, gs->graphics_data);
}

void IterateLinkedGamestates(t_linked_gamestate * first) {
	t_linked_gamestate * lgs = first;
	assert (lgs != NULL);
	t_gamestate * gs = lgs->gs;
	int return_code = 0;
	do {
		gs = lgs->gs;
		if ((lgs->flags & UPDATE) != 1){
			return_code = gs->f_update(lgs->gs->game_data);
			if (return_code != 0) {
				lgs->f_handle(return_code, lgs);
			}
		}
		if ((lgs->flags & DRAW) != 0) {
			gs->f_draw(lgs->gs->game_data, lgs->gs->graphics_data);
		}
		lgs = lgs->next;
	} while (lgs != NULL);	

}
