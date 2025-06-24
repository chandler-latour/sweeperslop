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
