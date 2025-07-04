#ifndef MINEFIELD
#define MINEFIELD
#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct s_minefield {
	unsigned int width;
	unsigned int height;
	unsigned int mine_count; //dimensions and basic facts of the board in tiles
	unsigned int * mine_locations; //integer array of mine coordinates
	unsigned int * revealed; //bitmap of tiles and whether they are revealed
	//1 means revealed, 0 means hidden.
	//actually a bitmap, an array of bits, not used as ints
} t_minefield;

enum VIEWTILES { //enum for all the visible label data for tiles
	FLAG_FALSE=-5, //sprite for the endgame for flags on safe tiles
	BUMP=-4,
	FLAG=-3,
	MINE_H=-2,
	H=-1,
	R=0,
	ONE=1,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	MINE=9
};

/*
A structure to point to a minefield and contain the tile proximities of the mines.
Separating this from the minefield itself allows me to make adjacency revealing happen
per-player rather than per-minefield, or potentially only allow a player to view one 
minefield at a time.
*/

typedef struct s_mineview {
	t_minefield * mf;
	signed char * proximities; //array of proximities of the mines
} t_mineview;

int CoordinateToIndex(t_minefield *, unsigned int, unsigned int);
t_bitaddr CoordinateToBitAddr(t_minefield *, unsigned int, unsigned int);


/*
Frees a mineview from memory.
*/
void FreeMineView(t_mineview *);

/*
Determines the numbers for the revealed tiles for an entire minefield, based on
which tiles on the minefield are revealed
*/
t_mineview ComputeMineView(t_minefield *);

/*
like the above but doesn't create a new mineview, instead takes an existing one.
*/
void UpdateMineView(t_mineview *);

/*
Computes the proximities of a single tile on a minefield.
*/
enum VIEWTILES ComputeTile(t_minefield *, unsigned int, unsigned int);

/*
The following function defines a mine field and allocates memory for an array of the landmines
it doesn't assign any landmine locations though.
*/
t_minefield NewMineField(unsigned int, unsigned int, unsigned int);

/*
The following function accepts a pointer to a minefield datatype and frees
the manually-allocated arrays inside of it.
*/
void FreeMineField(t_minefield *);

/*
The following function sets the minefield's entire board
to unrevealed space. Just zeroes out every int.
*/
void UnrevealBoard(t_minefield *);

/*
The following function populates a minefield with landmines at random
within the bounds of the board and the amount of mines specified.
It requires a pointer to a minefield that has a mine_locations array.
It doesn't matter if this mine_locations has mines already defined in it or not,
it just needs to exist and have memory allocated to it. 
TODO: decide whether to un-reveal the entire board upon doing this
*/
void PlantRandomMines(t_minefield *);

/*
The following function checks if a tile is revealed already, and if so, reveals it
returns an int based on how many mines are near the tile
returns 9 if it's a mine.
If the tile has 0 mines next to it, propagates this function down a few layers
Don't recursively propagate too much, because it could tank the framerate or 
crash the game. Also, perhaps the player should have an exploration vision radius
just feels realistic that way or something, idk

TODO: write one that propagates WITHOUT using recursion.
*/

enum VIEWTILES RevealTile(t_minefield *, unsigned int, unsigned int, unsigned char, unsigned char);


/*
 This function evaluates the game state of a board. It goes through every tile to check if it's revealed and safe.
 It returns an int for the number of mines which are unrevealed and safe.
 If it returns 0, that's a victory condition; it means every mine which has been revealed is safe
 If it finds a revealed tile with a mine in it, it prematurely returns -1, which means "loss" (you clicked on a mine!)
 * */

int EvaluateBoard(t_minefield *);

/*
This function reveals all mine tiles, and does nothing else.
This is classic minesweeper behavior when you lose the game.
*/

void RevealAllMines(t_minefield *);


/*
 * This function checks every flag. If a flag is placed upon a tile
 * that is not a mine, that flag is replaced with a "false flag" sprite
 * (a flag with a red X over it). This is also classic minesweeper
 * behavior when you lose the game.
 */
void RevealAllFlags(t_mineview *);

#endif
