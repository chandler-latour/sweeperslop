#include "raylib.h"
#include "minefield.h"

#include <assert.h>

/*
Creates a new minefield and allocates space to its various arrays
*/
t_minefield NewMineField(unsigned int w, unsigned int h, unsigned int m) {
	t_minefield mf;
	mf.width = w;
	mf.height = h;
	mf.mine_count = m;
	mf.mine_locations = malloc(sizeof(int) * m * 2); //a single-dimension array that gets used 2-dimensionally
	//so, it must have two entries for each mine. One int for x, one int for y.
	int bits = w * h; //malloc size in raw bits with no padding, but malloc doesn't take bits
	int bytes = bits / 8; //size in bytes, but not useful because we're assigning to an int pointer 
	int ints = bytes / sizeof(int); //size in ints, what we want to use but can't because...
	int gap = bits - (ints * sizeof(int) * 8); //...it would leave this gap of unaccounted bits!
	if (gap > 0) {
		ints = ints + 1;
		/*
		add an entire extra integer to contain the padding only if we need it
		we will not need more than the space of a single integer
		I wrote a program to test this personally, and I also have a math reason for it
		for starters, regardless of platform, int should always have a size in bytes!
		this is because malloc and all sorts of other standard C stuff wants a size in bytes!
		this means that if our bits divide evenly into bytes with no truncated remainder, then 
		the final gap would only be made up of complete bytes that don't form a full int
		then, combine this with the fact that those gap bytes simply have to be less than an int
		because that gap is a remainder of the division
		therefore, we will always have less than sizeof(int) space that needs to be accounted for
		so, just add one extra int and then don't worry about it
		*/
	}
	mf.revealed = malloc(ints * sizeof(int));
	assert(mf.mine_locations); //TODO: cook up a better way to deal with
	//unallocated minefields
	assert(mf.revealed);
	UnrevealBoard(&mf);
	return mf;
}


/*
The following function accepts a pointer to a minefield datatype and frees
the manually-allocated arrays inside of it.
*/
void FreeMineField(t_minefield * mf) {
	free(mf->mine_locations);
	free(mf->revealed);
	mf->mine_locations = NULL;
	mf->revealed = NULL;
};

/*
The following function sets the minefield's entire board
to unrevealed space. Just zeroes out every int.
*/
void UnrevealBoard(t_minefield * mf) {
	int w = mf->width;
	int h = mf->height;
	int arr_size = w * h;
	arr_size = arr_size / 8;
	arr_size = arr_size / sizeof(int);
	int gap = (w * h) - (arr_size * sizeof(int) * 8);
	if (gap > 0) {
		arr_size = arr_size + 1;
	}
	//TODO: extrapolate the above functionality into a function so it can be shared between multiple functions
	for (int i = 0; i < arr_size; i++) {
		mf->revealed[i] = 0x00000000;//zero out the integer to unreveal every mine
	}
}

/*
The following function populates a minefield with landmines at random
within the bounds of the board and the amount of mines specified.
It requires a pointer to a minefield that has a mine_locations array.
It doesn't matter if this mine_locations has mines already defined in it or not,
it just needs to exist and have memory allocated to it. 
TODO: decide whether to un-reveal the entire board upon doing this
*/

void PlantRandomMines(t_minefield * mf) {
	for (int i = 0; i < mf->mine_count; i++) {
		mf->mine_locations[i * 2] = GetRandomValue(0, mf->width - 1); //using a 1d array 2 dimensionally because I can't be bothered to malloc a bunch of mini arrays inside the main array
		mf->mine_locations[i * 2 + 1] = GetRandomValue(0, mf->height - 1);
		//Check if this mine is in the same place as every mine before it and, if so,
		//generate it someplace else until that isn't the case
		for (int j = 0; j < mf->mine_count; j++) {
			if (j == i) {
				continue;
			}
			int equals = (mf->mine_locations[i * 2] == mf->mine_locations[j * 2]);
			equals = equals && (mf->mine_locations[i * 2 + 1] == mf->mine_locations[j * 2 + 1]);
			if (equals) {
				//rewinds the incrementer
				//basically, this will reduce it, and then the for loop evaluation will increment it
				//again, keeping it the same, so we elegantly re-generate the current mine
				i--;
				break;
			}
		}
	}
}
//This function converts an X, Y coordinate on a minesweeper board to a bit index
int CoordinateToIndex(t_minefield * mf, unsigned int x, unsigned int y) {
	return y * mf->width + x;
}
//This function uses the previous one to convert an X, Y coordinate on a board to a bit address.
t_bitaddr CoordinateToBitAddr(t_minefield * mf, unsigned int x, unsigned int y) {
	return BitIndexToBitAddr(CoordinateToIndex(mf, x, y));
}

/*
This function creates a mineview using the following function on every single tile in a minefield.
It doesn't reveal any un-revealed tiles, just creates and allocates memory for the mineview
and fills in information.
*/
t_mineview ComputeMineView(t_minefield * mf) {
	t_mineview mv;
	mv.mf = mf;
	mv.proximities = malloc(mf->width * mf->height * sizeof(char));
	assert(mv.proximities);
	UpdateMineView(&mv);
	return mv;
}

/*
this function updates a mineview without creating a new one.
*/
void UpdateMineView(t_mineview * mv) {
	unsigned int w = mv->mf->width;
	unsigned int h = mv->mf->height;
	signed char prox = 0;	
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			prox = mv->proximities[i + j * w];
			if (prox == FLAG || prox == FLAG_FALSE) {
				continue;
			}
			mv->proximities[i + j * w] = ComputeTile(mv->mf, i, j);
		}
	}
}

/*
The following function takes a minefield and a given cell on the board and computes it's proximities to mines.
If the cell is currently hidden, don't reveal it, just return hidden. (using enum VIEWTILES H).
If it's a revealed mine, return enum VIEWTILES MINE.
Otherwise, reveal proximities (including 0 if it's empty and has no mines around).
This function is used by multiple others, which is why it has to have this flexible behavior.
*/

enum VIEWTILES ComputeTile(t_minefield * mf, unsigned int x, unsigned int y) {
	t_bitaddr ba;
	ba = CoordinateToBitAddr(mf, x, y);
	//check if the desired tile is revealed
	unsigned int masked_int = mf->revealed[ba.int_index] & ba.bit_mask;
	//every other tile within this int is masked to 0, except the one we want to check
	//so, if the one we want to check is 0, masked_int=0
	//otherwise, it'll have a non-zero value
	if (masked_int == 0) {
		return H;
	}

	//variables for the adjacent tile bounds around this tile
	//based on whether it's a wall or corner mine or not
	//TODO: extrapolate this into a function somehow so I can reuse it more easily
	int x_start = (x > 0) ? -1 : 0;
	int x_end = (x < mf->width-1) ? 1 : 0;
	int y_start = (y > 0) ? -1 : 0;
	int y_end = (y < mf->height-1) ? 1 : 0;	
	int mine_adj_count = 0;
	for (int i = x_start; i < x_end + 1; i++) {
		for (int j = y_start; j < y_end + 1; j++) {
			for (int k = 0; k < mf->mine_count; k++) {
				int mine_x = mf->mine_locations[k*2];
				int mine_y = mf->mine_locations[k*2+1];
				if (mine_x == x+i && mine_y == y+j) {
					//there's a mine on the current tile
					//now we gotta check if it's the tile being computed or an adjacent one
					if (i == 0 && j == 0) {
						//(0, 0) relative is THIS TILE!
						//so if there's a mine on this tile, return it!
						return MINE;
					}
					else {
						//there's a mine on an adjacent tile, so increment the counter
						mine_adj_count++;
					}
				}
			}
		}
	}
	return mine_adj_count;
}

/*
The following function checks if a tile is revealed already, and if so, reveals it
returns an int based on how many mines are near the tile
returns 9 (ENUM VIEWTILES MINE) if it's a mine.
If the tile has 0 mines next to it, propagates this function down a few layers
Don't recursively propagate too much, because it could tank the framerate or 
crash the game. Also, perhaps the player should have an exploration vision radius
just feels realistic that way or something, idk
this function is more than just Compute Tile because compute tiles doesn't 
reveal an already unrevealed tile, while this one does. That's a big deal because
you can use compute tile to iterate over an entire minefield without 
revealing the entire minefield and stepping on all the landmines
that function is great for if the player gets one "mineview" datastructure and I need
to load the minefields in and out of that data structure elegantly while 
the minefield remains undisturbed though it
by contrast, this function is designed for actually engaging in gameplay and revealing mines. 

TODO: write one that propagates WITHOUT using recursion.
*/
enum VIEWTILES RevealTile(t_minefield * mf, unsigned int x, unsigned int y, unsigned char propagate) {
	t_bitaddr ba = CoordinateToBitAddr(mf, x, y);
	mf->revealed[ba.int_index] = mf->revealed[ba.int_index] | ba.bit_mask; //a bitwise operation to turn that bit to a 1
	
	/*
	int return_val = 0;
	for (int i = 0; i < mf->mine_count; i++) {
		int mine_x = mf->mine_locations[i*2];
		int mine_y = mf->mine_locations[i*2+1];
		if (mine_x = x && mine_y = y) {
			return enum VIEWTILES MINE; //mine on the revealed tile
		}
		else if(mine_x > x-2 && mine_x < x+2 && mine_y > y-2 && mine_y < y+2) {
				return_val++;
			}	
		}
	}
	*/
	int return_val = ComputeTile(mf, x, y);
	if (return_val == MINE) {
		return return_val; //return the mine to the game so it can decide how to deal with the ensuing explosion.
	} 
	if (return_val == 0 && propagate == 1) {
		unsigned int max_tiles = (mf->width * mf->height - mf->mine_count) * 2 ;
		unsigned int empty_tile_count = 1;
		unsigned int empty_tiles[max_tiles]; //stores the x and y values of the empty tiles to be checked
						     //it can't be larger than the amount of tiles on the board minus the mines
						     //but multiply that times 2 because we have 2 entries per tile, x and y
		for (int i = 0; i < max_tiles; i++) {
			empty_tiles[i] = 0;
		}
		empty_tiles[0] = x;
		empty_tiles[1] = y;
		//if there are no mines anywhere around this square, propagate!
		//iterate through each of the 8 neighboring cells, skipping the center cells
		//dynamic bound checking to account for corners and walls on the edge of the minefield.
		int x_start = (x > 0) ? -1 : 0;
		int x_end = (x < mf->width-1) ? 1 : 0;
		int y_start = (y > 0) ? -1 : 0;
		int y_end = (y < mf->height-1) ? 1 : 0;
		unsigned int tx = x; //short for "this x"
		unsigned int ty = y;
		int masked_int = 0;
		int reveal_return = 0;
		for (int tile_index = 0; tile_index < empty_tile_count; tile_index++) {
			tx = empty_tiles[tile_index * 2];
			ty = empty_tiles[tile_index * 2 + 1];
			x_start = (tx > 0) ? -1 : 0;
			x_end = (tx < mf->width-1) ? 1 : 0;
			y_start = (ty > 0) ? -1 : 0;
			y_end = (ty < mf->height-1) ? 1 : 0;
			for (int i = x_start; i < x_end + 1; i++) {
				for (int j = y_start; j < y_end + 1; j++) {
					if (i == 0 && j == 0) {
						continue; //(0, 0) is relative to this tile means it is this tile
					}
					ba = CoordinateToBitAddr(mf, tx+i, ty+j);
					masked_int = mf->revealed[ba.int_index] & ba.bit_mask;
					if (masked_int == 0) {
						reveal_return = RevealTile(mf, tx + i, ty + j, 0); //doesn't propagate, just returns the tile
						if (reveal_return == 0) {
							int ctx = 0;
							int cty = 0;
							int duped = 0;
							for (int k = 0; k < empty_tile_count; k++) {
								ctx = empty_tiles[k * 2];
								cty = empty_tiles[k * 2 + 1];
								if (ctx == tx + i && cty == ty + j) {
									//revealed tile is already in array, no need to add it
									duped = 1;
									break;
								}
							}
							if (duped == 0) {
								empty_tiles[empty_tile_count * 2] = tx + i;
								empty_tiles[empty_tile_count * 2 + 1] = ty + j;
								empty_tile_count++;
							}
						}
					}
				}
			}
		}
	}
	return return_val;
}

//this one frees the memory occupied by a mineview
void FreeMineView(t_mineview * mv) {
	free(mv->proximities);
	mv->proximities = NULL;
}

/*
 This function evaluates the game state of a board. It goes through every tile to check if it's revealed and safe.
 It returns an int for the number of mines which are unrevealed and safe.
 If it returns 0, that's a victory condition; it means every mine which has been revealed is safe
 If it finds a revealed tile with a mine in it, it prematurely returns -1, which means "loss" (you clicked on a mine!)
 * */

int EvaluateBoard(t_minefield * mf) {
	int tiles_remaining = mf->width * mf->height - mf->mine_count;
	t_bitaddr ba;
	int tile_rev = 0; //integer to store whether this tile has been revealed
	int mine_x = 0;
	int mine_y = 0;
	for (int i = 0; i < mf->width; i++) {
		for (int j = 0; j < mf->height; j++) {
			ba = CoordinateToBitAddr(mf, i, j);
			tile_rev = mf->revealed[ba.int_index] & ba.bit_mask; //AND masks the bit we want to check
			//every other bit bit becomes zero, this bit stays as it is
			//so if it's 0, the whole int evaluates that way
			//NOTE: a revealed tile can evaluate as many different non-zero numbers, not just 1!
			//This is because I didn't clamp that flipped bit down to the ones place.
			//So, up ahead I check > 0 rather than == 1
			if (tile_rev > 0) {
				for (int m = 0; m < mf->mine_count; m++) {
					mine_x = mf->mine_locations[m * 2];
					mine_y = mf->mine_locations[m * 2 + 1];
					if (mine_x == i && mine_y == j) {
						return -1;
						//Found a revealed mine, oopsies!
					}
				}
// If we made it this far, it means we found a tile that is revealed and lacks a mine.
				tiles_remaining -= 1; 
			}
		}
	}
	return tiles_remaining;
}

/*
 * This function reveals all mine tiles, and does nothing else.
 * This is classic minesweeper behavior when you lose the game.
 */

void RevealAllMines(t_minefield * mf) {
	for (int m = 0; m < mf->mine_count; m++) {
		RevealTile(mf, mf->mine_locations[m * 2], mf->mine_locations[m * 2 + 1], 0);
	}
}


 /*
 * This function checks every flag. If a flag is placed upon a tile
 * that is not a mine, that flag is replaced with a "false flag" sprite
 * (a flag with a red X over it). This is also classic minesweeper
 * behavior when you lose the game.
 */
void RevealAllFlags(t_mineview * mv) {
	unsigned int len = mv->mf->width * mv->mf->height;
	int valid_flag = 1;
	int mine_x = 0;
	int mine_y = 0;
	int tile_x = 0;
	int tile_y = 0;
	for (int i = 0; i < len; i++) {
		valid_flag = 1;
		tile_y = i / mv->mf->width;
		tile_x = i % mv->mf->width;
		if (mv->proximities[i] == FLAG) {
			valid_flag = 0;
			for (int i = 0; i < mv->mf->mine_count; i++) {
				mine_x = mv->mf->mine_locations[i * 2];
				mine_y = mv->mf->mine_locations[i * 2 + 1];
				if (mine_y == tile_y && mine_x == tile_x) {
					valid_flag = 1;
					break;
				}
			}
			if (valid_flag == 0) {
				mv->proximities[i] = FLAG_FALSE;
			}
		}
	}
}

  

				
				
				
