/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Requires: <stdbool.h>, <SDL/SDL.h>
 *
 *	Definitions for the game board.
 */

#ifndef BOARD_H
#define BOARD_H

#define BD_W		10		// Board size, in blocks
#define BD_H		20
#define BD_COLS		8		// Number of colours

// Block colours, CLEAR means no block
typedef enum { 
	CLEAR = 0, BLUE, CYAN, GREEN, PURPLE, RED, ORANGE, YELLOW
} bd_col_t;

// Function prototypes
extern void bd_copytobd(int x, int y, bd_col_t col);
extern bool bd_iscollide(int x, int y);
extern unsigned bd_chkfull(unsigned start, unsigned end);
extern void bd_chkrm(void);
extern void bd_draw(SDL_Surface *screen, SDL_Surface *blocks);
extern bool bd_isoff(int x, int y);
extern void bd_drawblk(SDL_Surface *screen, SDL_Surface *blocks, bd_col_t col, 
		int x, int y, bool flash);
extern void bd_init(void);

#endif // BOARD_H
