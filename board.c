/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Code for the game board data structure.
 */

#include <assert.h>
#include <stdbool.h>
#include "SDL.h"
#include "bloc.h"
#include "board.h"

#define BD_BGX			24		// Boards position on background bitmap
#define BD_BGY			144
#define BD_BLKW			24		// Dimensions of block sprite
#define BD_BLKH			24
#define BD_LINETICKS	20		// Number of ticks till full line is removed
#define BD_ANIMDIV		4		// How often line is flashed

/*
 *	The game board is a two-dimensional array of coloured blocks.
 */
static bd_col_t brd[BD_H][BD_W];

/*
 *	Line is removed once its ticks reaches zero, animation is based on number 
 *	of ticks left.
 */
static unsigned lineticks[BD_H];

/*
 *	Copy the block to the board at the given position.
 */
void 
bd_copytobd(int x, int y, bd_col_t col) {
	brd[y][x] = col;
}

/*
 *	Returns true if the given block is not clear.
 */
bool
bd_iscollide(int x, int y) {
	return brd[y][x] != CLEAR;
}

/*
 *	Check board for full lines and start the counter for animation and line 
 *	removal.  Returns the number of full lines.
 *	start	- line to start checking from
 *	end		- line to stop checking on
 */
unsigned
bd_chkfull(unsigned start, unsigned end) {
	bool isfull;			// Is this a full line?
	unsigned lines = 0;		// Number of full line

	for (unsigned j = start; j <= MIN(end, BD_H - 1); j++) {
		isfull = true;
		for (unsigned i = 0; i < BD_W; i++) {
			if (brd[j][i] == CLEAR) {
				isfull = false;
				break;
			}
		}
		if (isfull) {
			lineticks[j] = BD_LINETICKS;
			lines++;
		}
	}
	return lines;
}

/*
 *	Check board for lines to remove, line is removed once lineticks[i] reaches
 *	zero.
 */
void 
bd_chkrm(void) {
	int skip = 0;	// Number of lines to skip when moving lines down

	for (int j = BD_H - 1; j >= skip; j--) {
		for (int k = j - skip; k >= skip; k--) {
			if (lineticks[k] > 0 && --lineticks[k] == 0) {
				skip++;
			} else {
				break;
			}
		}
		if (skip > 0) {
			for (int i = 0; i < BD_W; i++) {
				brd[j][i] = brd[j-skip][i];
			}
			lineticks[j] = lineticks[j-skip];
		}
	}
	for (int j = 0; j < skip; j++) {
		for (int i = 0; i < BD_W; i++) {
			brd[j][i] = CLEAR;
		}
		lineticks[j] = 0;
	}
	/* Working but less efficient algorithm, lines can be moved multiple times:
	for (int i = 0; i < BD_H; i++) {
		if (lineticks[i] > 0) {
			if (--lineticks[i] == 0) {
				for (int j = i; j >= 0; j--) {
					for (int k = 0; k < BD_W; k++) {
						brd[j][k] = brd[j-1][k];
					}
					lineticks[j] = lineticks[j-1];
				}
				for (int k = 0; k < BD_W; k++) {
					brd[0][k] = CLEAR;
				}
				lineticks[0] = 0;
			}
		}
	}
	*/
}

/*
 *	Draw the game board, flashes full lines that are about to be removed.
 *	screen - screen surface
 *	blocks - blocks bitmap
 */
void
bd_draw(SDL_Surface *screen, SDL_Surface *blocks) {
	bd_col_t col;
	bool flash;

	assert(screen != NULL && blocks != NULL);
	for (int j = 0; j < BD_H; j++) {
		if (lineticks[j] > 0) {
			flash = (((lineticks[j] - 1) / BD_ANIMDIV) % 2 == 0);
		} else {
			flash = false;
		}
		for (int i = 0; i < BD_W; i++) {
			col = brd[j][i];
			if (col != CLEAR) {
				bd_drawblk(screen, blocks, col, i, j, flash);
			}
		}
	}
}

/*
 *	Returns true if the co-ordinates are off the game board, false otherwise.
 */
bool
bd_isoff(int x, int y) {
	return (x < 0
		||  x >= BD_W
		||  y < 0
		||  y >= BD_H);
}

/*
 *	Blit a certain colour block at a given position on the board.
 *	screen	- screen surface
 *	blocks	- blocks bitmap
 *	flash	- true if flash animated block
 */
void
bd_drawblk(SDL_Surface *screen, SDL_Surface *blocks, bd_col_t col, int x, 
		int y, bool flash) {
	SDL_Rect srcrect = {
		(Sint16) (col - 1) * BD_BLKW,
		(Sint16) (flash) ? BD_BLKH : 0,
		(Uint16) BD_BLKW,
		(Uint16) BD_BLKH
	};
	SDL_Rect dstrect = {
		(Sint16) BD_BGX + x * BD_BLKW,
		(Sint16) BD_BGY + y * BD_BLKH,
		0,	// Unused
		0	// Unused
	};

	assert(screen != NULL && blocks != NULL);
	if (SDL_BlitSurface(blocks, &srcrect, screen, &dstrect) != 0) {
		b_error("Error blitting block: %s\n", SDL_GetError());
	}
}

/*
 *	Clear the board.
 */
void 
bd_init(void) {
	for (int j = 0; j < BD_H; j++) {
		for (int i = 0; i < BD_W; i++) {
			brd[j][i] = CLEAR;
		}
	}
}
