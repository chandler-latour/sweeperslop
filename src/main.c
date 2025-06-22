#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "raylib.h"

#include "minefield.h"
#include "bitmap.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

typedef struct s_test {
	int frame;
	char str[100];
} t_test;

void UpdateDrawFrame(void *);
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "SWEEPER SLOP");

    //--------------------------------------------------------------------------------------

	//set up minesweeper board
	t_minefield mf = NewMineField(9, 9, 10);
	PlantRandomMines(&mf);
	t_mineview mv = ComputeMineView(&mf);	

    // Main game loop
	Texture2D tile = LoadTexture("../assets/tex/unrevealed_tile.png");
	Texture2D tile_bump = LoadTexture("../assets/tex/bump_tile.png");
	Texture2D tiles_r[14] = {
		LoadTexture("../assets/tex/bump_tile.png"),
		LoadTexture("../assets/tex/flag_tile.png"),
		LoadTexture("../assets/tex/unrevealed_tile.png"), //TODO: draw unrevealed tile with mine on it and put it here
		LoadTexture("../assets/tex/unrevealed_tile.png"),
		LoadTexture("../assets/tex/blank_tile.png"),
		LoadTexture("../assets/tex/one_tile.png"),
		LoadTexture("../assets/tex/two_tile.png"),
		LoadTexture("../assets/tex/three_tile.png"),
		LoadTexture("../assets/tex/four_tile.png"),
		LoadTexture("../assets/tex/five_tile.png"),
		LoadTexture("../assets/tex/six_tile.png"),
		LoadTexture("../assets/tex/seven_tile.png"),
		LoadTexture("../assets/tex/eight_tile.png"),
		LoadTexture("../assets/tex/mine_tile.png")};

	int s_x = 0;
	int s_y = 0;
	printf("done loading stuff!\n");
	t_test t = {};
	t.frame = 0;
	for (int i = 0; i < 100; i++) {
		t.str[i] = '\0';
	}
	#ifdef PLATFORM_WEB
	emscripten_set_main_loop_arg(UpdateDrawFrame, &t, 0, 1);

	#else
	SetTargetFPS(60);
	while (!WindowShouldClose()) {   // Detect window close button or ESC key
		UpdateDrawFrame(&t);
	}
	#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
	FreeMineView(&mv);
	FreeMineField(&mf);
	UnloadTexture(tile);
	UnloadTexture(tile_bump);
	for (int i = 0; i < 10; i++) {
		UnloadTexture(tiles_r[i]);
	}
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdateDrawFrame(void * tpv) {
	t_test * tp = (t_test *) tpv;
	double dt = GetFrameTime();
	int fps = GetFPS();
	sprintf(tp->str, "frame:%u\ndt:%f\nfps:%u", tp->frame, dt, fps);
	tp->frame += 1;

	BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText(tp->str, 20, 20, 16, DARKGRAY);
	EndDrawing();
}
