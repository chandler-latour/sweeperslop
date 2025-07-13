#include "gs_classic.h"
#include "gamestate.h"
#include "minefield.h"
#include "raylib.h"


/*
This function accepts a pointer to a board data and prepares it with the arguments.
*/
void PrepareBoardData(t_boarddata * bd, float x, float y, unsigned int width, unsigned int height, unsigned int mines) {
	bd->position = (Vector2) {x, y};
	bd->mf = NewMineField(width, height, mines);
	PlantRandomMines(&(bd->mf));
	bd->mv = ComputeMineView(&(bd->mf));
}


/*
This function accepts the width, height, and mine count of a classic minesweeper board,
and returns a gamestate running that board with a randomly generated assortment of mines.
NOT RECOMMENDED, DEPRECIATED, DO NOT USE.
It creates a bd on the stack, but memory addresses change when you return it, so the internal pointers break!
*/

t_boarddata NewBoardData(float x, float y, unsigned int width, unsigned int height, unsigned int mines) {
	t_boarddata bd;
	PrepareBoardData(&bd, x, y, width, height, mines);
	return bd;
}

//frees all the memory associated with the minefield and mineview gamestate
void CloseClassicMineBoard(t_gamestate * gs) {
	FreeBoardData(gs->game_data);	
	UnloadMinesweeperGFX(gs->graphics_data);
}

void FreeBoardData(t_boarddata * bd) {
	FreeMineField(&(bd->mf));
	FreeMineView(&(bd->mv));
}

t_vec2int MouseToTile(float bdx, float bdy, unsigned int width, unsigned int height) {
	int s_x = (GetMouseX() - bdx)/TILE_SIZE;
	int s_y = (GetMouseY() - bdy)/TILE_SIZE;
	//bounds checking for selected tile
	s_x = (s_x < 0) ? 0 : s_x;
	s_x = (s_x > width - 1) ? width - 1 : s_x;
	s_y = (s_y < 0) ? 0 : s_y;
	s_y = (s_y > height - 1) ? height - 1 : s_y;
	return (t_vec2int) {s_x, s_y};
}

//updates the minefield, allows the player to click tiles or flag 'em
void UpdateClassicMineBoard(void * bdv) {
	t_boarddata * bd = (t_boarddata *) bdv;

	t_minefield * mf = &(bd->mf);
	t_mineview * mv = &(bd->mv);
	t_vec2int mouse_tile = MouseToTile(bd->position.x, bd->position.y, mf->width, mf->height);
	int s_x = mouse_tile.x;
	int s_y = mouse_tile.y;
	
	signed char * selected_tile = &(mv->proximities[s_x + s_y * mf->width]);

	int game_state = EvaluateBoard(mf);
	if (game_state > 0) {
		if (IsMouseButtonPressed(0) && *selected_tile != FLAG) {
			int prox = RevealTile(mf, s_x, s_y, 1);
			if (prox == MINE) {
				RevealAllMines(mf);
				RevealAllFlags(mv);
			}
			UpdateMineView(mv);
			game_state = EvaluateBoard(mf);
		}
		if (IsMouseButtonPressed(1) && *selected_tile == H) {
			*selected_tile = FLAG;
		}
		else if (IsMouseButtonPressed(1) && *selected_tile == FLAG) {
			*selected_tile = H;
		}
	}	
}

t_minesweepergfx LoadMinesweeperGFX() {
	t_minesweepergfx gfx;
	gfx.tiles[0]  = LoadTexture("../assets/tex/flag_false_tile.png");
	gfx.tiles[1]  = LoadTexture("../assets/tex/bump_tile.png");
	gfx.tiles[2]  = LoadTexture("../assets/tex/flag_tile.png");
	gfx.tiles[3]  = LoadTexture("../assets/tex/unrevealed_tile.png"); //TODO: draw unrevealed tile with mine on it and put it here
	gfx.tiles[4]  = LoadTexture("../assets/tex/unrevealed_tile.png");
	gfx.tiles[5]  = LoadTexture("../assets/tex/blank_tile.png");
	gfx.tiles[6]  = LoadTexture("../assets/tex/one_tile.png");
	gfx.tiles[7]  = LoadTexture("../assets/tex/two_tile.png");
	gfx.tiles[8]  = LoadTexture("../assets/tex/three_tile.png");
	gfx.tiles[9]  = LoadTexture("../assets/tex/four_tile.png");
	gfx.tiles[10] = LoadTexture("../assets/tex/five_tile.png");
	gfx.tiles[11] = LoadTexture("../assets/tex/six_tile.png");
	gfx.tiles[12] = LoadTexture("../assets/tex/seven_tile.png");
	gfx.tiles[13] = LoadTexture("../assets/tex/eight_tile.png");
	gfx.tiles[14] = LoadTexture("../assets/tex/mine_tile.png");
	return gfx;
}

void UnloadMinesweeperGFX(t_minesweepergfx * gfx) {
	for (int i = 0; i < TILE_SPRITE_COUNT; i++) {
		UnloadTexture(gfx->tiles[i]);
	}
}

//draws the minefield
void DrawClassicMineBoard(void * bdv, void * gfxv) {
	t_minesweepergfx * gfx = (t_minesweepergfx *) gfxv;
	t_boarddata * bd = (t_boarddata *) bdv;
	float bdx = bd->position.x;
	float bdy = bd->position.y;
	t_minefield * mf = &(bd->mf);
    t_mineview * mv = &(bd->mv);

	t_vec2int mouse_tile = MouseToTile(bd->position.x, bd->position.y, mf->width, mf->height);
	int s_x = mouse_tile.x;
	int s_y = mouse_tile.y;
	
	BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawText("minesweeper go BRRRRRRRRR!", 8, 8, 20, LIGHTGRAY);
            for (int i = 0; i < mf->width; i++) {
                for (int j = 0; j < mf->height; j++) {
                    signed char prox = mv->proximities[i + j * mf->width];
					//TODO I GOTTA GET THE TEXTURES INTO THE FUNCTION SOMEHOW
                    DrawTexture(gfx->tiles[prox + 5], i*TILE_SIZE + bdx, j * TILE_SIZE + bdy, YELLOW); //TODO get rid of magic 5 constant (it's for the negative enum offset)
                }
            }
            //draw selector
            DrawCircle(s_x*TILE_SIZE + bdx, s_y*TILE_SIZE + bdy, 5, DARKBLUE);
	EndDrawing();
}

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
*/
