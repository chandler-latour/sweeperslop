#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "raylib.h"

#include "minefield.h"
#include "bitmap.h"
#include "gamestate.h"
#include "gs_classic.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
/*
typedef struct s_test {
	int frame;
	char str[100];
} t_test;
*/

void UpdateDrawFrame(void *);
int main(int argc, char ** argv)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 640;
    int screenHeight = 480;
	int mines = 10;
	int width = 9;
	int height = 9;
	printf("Starting with %d arguments\n", argc);
	if (argc == 4) {
		int stat = 0;
		stat += sscanf(argv[1], "%d", &width);
		stat += sscanf(argv[2], "%d", &height);
		stat += sscanf(argv[3], "%d", &mines);
		printf("Read %d arguments\n", stat);
		if (stat != 3) {
			return -1;
		}
		screenWidth = TILE_SIZE * width + 64;
		screenHeight = TILE_SIZE * height + 64;
	}

    InitWindow(screenWidth, screenHeight, "SWEEPER SLOP");

    //--------------------------------------------------------------------------------------

	//set up minesweeper board
	t_boarddata bd;
	PrepareBoardData(&bd, 16, 48, width, height, mines);
	//bd has sub structures with pointers that point to other substructures in bd
	//this is a complication because if I use a function that simply returns a bd,
	//all the pointers will be wrong because the memory address of the bd that returns
	//will be different than the one in the function
	//I want to keep some strucutural stuff on the stack when I can, so I cooked up
	//this function which wants a reference to a bd that already exists instead.
	//This is a much more flexible option because if I decide I want to malloc some board datas later,
	//it can handle that too!

	t_minesweepergfx gfx = LoadMinesweeperGFX();
	
	t_gamestate classic_gs;
	classic_gs.f_update = &UpdateClassicMineBoard;
	classic_gs.f_draw = &DrawClassicMineBoard;
	classic_gs.f_close = &CloseClassicMineBoard;
	classic_gs.game_data = &bd;
	classic_gs.graphics_data = &gfx;
	
	#ifdef PLATFORM_WEB
	emscripten_set_main_loop_arg(comboloop, &classic_gs, 0, 1);

	#else
	SetTargetFPS(60);
    	while (!WindowShouldClose()) {   // Detect window close button or ESC key
		comboloop(&classic_gs);
	}
	#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    classic_gs.f_close(&classic_gs);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
