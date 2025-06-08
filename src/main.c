#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "raylib.h"

#include "minefield.h"
#include "bitmap.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "SWEEPER SLOP");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

	//set up minesweeper board
	t_minefield mf = NewMineField(9, 9, 10);
	PlantRandomMines(&mf);
	t_mineview mv = ComputeMineView(&mf);	

    // Main game loop
	Texture2D tile = LoadTexture("../assets/tex/unrevealed_tile.png");
	Texture2D tile_bump = LoadTexture("../assets/tex/bump_tile.png");
	Texture2D tiles_r[10] = {
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
	
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

		s_x = (GetMouseX() - 32)/32;
		s_y = (GetMouseY() - 32)/32;
		s_x = (s_x < 0) ? 0 : s_x;
		s_x = (s_x > mv.mf->width - 1) ? mv.mf->width - 1 : s_x;
		s_y = (s_y < 0) ? 0 : s_y;
		s_y = (s_y > mv.mf->height - 1) ? mv.mf->height - 1 : s_y;
		
		
		//move the selector
		if (IsKeyPressed(KEY_UP) && s_y > 0) {
			s_y = s_y - 1;
		}
		if (IsKeyPressed(KEY_DOWN) && s_y < mv.mf->height - 1) {
			s_y = s_y + 1;
		}
		if (IsKeyPressed(KEY_LEFT) && s_x > 0) {
			s_x = s_x - 1;
		}
		if (IsKeyPressed(KEY_RIGHT) && s_x < mv.mf->width - 1) {
			s_x = s_x + 1;
		}
		if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(0)) {
			int prox = RevealTile(mv.mf, s_x, s_y, 0, 10);
			UpdateMineView(&mv);
		}
	
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("minesweeper go BRRRRRRRRR!", 8, 8, 20, LIGHTGRAY);
			//draw minefield	
			for (int i = 0; i < mv.mf->width; i++) {
				for (int j = 0; j < mv.mf->height; j++) {
					signed char prox = mv.proximities[i + j * mv.mf->width];
					if (prox == -1) {
						DrawTexture(tile, i*32 + 32, j * 32 + 32, YELLOW);
					}
					else if (prox < 10) {
						DrawTexture(tiles_r[prox], i*32 + 32, j * 32 + 32, YELLOW);
					}
				}
			}
			//draw selector
			DrawCircle(s_x*32 + 32, s_y*32 + 32, 5, DARKBLUE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

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
