//This is a header for all the functions needed to run a classic minesweeper game

#ifndef CLASSIC_GAMESTATE
#define CLASSIC_GAMESTATE
#include "raylib.h"
#include "gamestate.h"
#include "minefield.h"

#define TILE_SPRITE_COUNT 15 //MAKE SURE TO CHANGE THIS
#define TILE_SIZE 32
//WHEN YOU ADD MORE TEXTURES!

typedef struct s_vec2int {
	unsigned int x;
	unsigned int y;
} t_vec2int;

//game data for the minesweeper board
typedef struct s_boarddata {
	Vector2 position; //global position of this minefield
	t_minefield mf; //the minefield in question
	t_mineview mv; //the mineview for the minefield
} t_boarddata;

void PrepareBoardData(t_boarddata *, float, float, unsigned int, unsigned int, unsigned int);

t_boarddata NewBoardData(float, float, unsigned int, unsigned int, unsigned int);

typedef struct s_minesweepergfx {
	Texture2D tiles[TILE_SPRITE_COUNT];//array of minesweeper tiles
} t_minesweepergfx;

/*
This function accepts the width, height, and mine count of a classic minesweeper board,
and returns a gamestate running that board with a randomly generated assortment of mines.
It takes the x and y position (float) of the board on the screen, the dimensions of 
the board (first two unsigned ints), and last the number of mines.
*/

t_gamestate NewClassicMineBoard(float, float, unsigned int, unsigned int, unsigned int);

//frees all the memory associated with the minefield and mineview gamestate
void CloseClassicMineBoard(t_gamestate *);

//updates the minefield, allows the player to click tiles or flag 'em
int UpdateClassicMineBoard(void *);

enum CLASSIC_GAMESTATE_RETURN {
	CGS_CONTINUE = 0,
	CGS_WIN = 1,
	CGS_FAIL = -1
};

//Handles the return code from a classic game state.
void HandleClassicMineBoard(int, t_linked_gamestate *);

//draws the minefield
void DrawClassicMineBoard(void *, void *);

void FreeBoardData(t_boarddata *);


//creates graphics structure for a minesweeper board
//this is optional if you already have another one that you can use
t_minesweepergfx LoadMinesweeperGFX();

void UnloadMinesweeperGFX(t_minesweepergfx * gfx);


//converts the mouse coordinates to tile coordinates
t_vec2int MouseToTile(float, float, unsigned int, unsigned int);

/*
mock main

t_gamestate gs_classic;

gs_classic.f_update = &UpdateClassicMineBoard;
gs_classic.f_draw = &DrawClassicMineBoard;

t_boarddata bd = NewBoard(x, y, width, height, mines);

gs_classic.f_close = &FreeBoardData;
gs_classic.game_data = &bd;
emscripten_assign_main_loop_args(comboloop, &gs_classic, 0, 1);

end mock main

mock NewBoard
t_boarddata NewClassicMineBoard(float x, float y, unsigned int width, unsigned int height, unsigned int mines) {
	t_boarddata bd;
	bd.position = {x, y};
	bd.mf = NewMineField(width, height, mines);
	PlantRandomMines(&(bd.mf));
	UnrevealBoard(&(bd.mf));
	bd.mv = ComputeMineView(&(bd.mf));
	return bd;
}



*/
#endif
