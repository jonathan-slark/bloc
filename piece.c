/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Code for the current and next game pieces data structure.
 */

#include <assert.h>
#include <stdbool.h>
#include "SDL.h"
#include "audio.h"
#include "board.h"
#include "piece.h"

#define P_W		4		// Piece size, in blocks
#define P_H		4
#define P_XORG	3		// Game piece starting position
#define P_YORG	0
#define P_NEXTX	12		// Next piece's position relative to the board
#define P_NEXTY	1

// Tetrimino game piece
typedef struct {
	bd_col_t	col;	// Colour
	p_rot_t		rot;	// Rotation
	int			x;		// X position, in blocks
	int			y;		// Y position
} piece_t;

// The main game piece
static piece_t piece;

// The next game piece
static piece_t nextpiece;

/*
 *	Pre-baked block data for Tetriminos game pieces.  Each piece is associated 
 *	with a 4x4 array of blocks.  Each array of blocks is indexed by the pieces 
 *	colour and rotation, making a four-dimensional array.  The colour CLEAR is 
 *	included to make indexing easier.
 */
static const bd_col_t p_blocks[BD_COLS][P_ROTS][P_H][P_W] = {
	{
		// CLEAR
		{
			{ 0 }
		}
	}, {
		// BLUE
		{
			// NORTH
			{ BLUE,		BLUE,	BLUE,	CLEAR },
			{ CLEAR,	CLEAR,	BLUE,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ CLEAR,	BLUE,	CLEAR,	CLEAR },
			{ CLEAR,	BLUE,	CLEAR,	CLEAR },
			{ BLUE,		BLUE,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// SOUTH
			{ BLUE,		CLEAR,	CLEAR,	CLEAR },
			{ BLUE,		BLUE,	BLUE,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ BLUE,		BLUE,	CLEAR,	CLEAR },
			{ BLUE,		CLEAR,	CLEAR,	CLEAR },
			{ BLUE,		CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}
	}, {
		// CYAN
		{
			// NORTH
			{ CYAN,		CYAN,	CYAN,	CYAN },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ CLEAR,	CYAN,	CLEAR,	CLEAR },
			{ CLEAR,	CYAN,	CLEAR,	CLEAR },
			{ CLEAR,	CYAN,	CLEAR,	CLEAR },
			{ CLEAR,	CYAN,	CLEAR,	CLEAR }
		}, {
			// SOUTH
			{ CYAN,		CYAN,	CYAN,	CYAN },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ CLEAR,	CYAN,	CLEAR,	CLEAR },
			{ CLEAR,	CYAN,	CLEAR,	CLEAR },
			{ CLEAR,	CYAN,	CLEAR,	CLEAR },
			{ CLEAR,	CYAN,	CLEAR,	CLEAR }
		}
	}, {
		// GREEN
		{
			// NORTH
			{ CLEAR,	GREEN,	GREEN,	CLEAR },
			{ GREEN,	GREEN,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ GREEN,	CLEAR,	CLEAR,	CLEAR },
			{ GREEN,	GREEN,	CLEAR,	CLEAR },
			{ CLEAR,	GREEN,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// SOUTH
			{ CLEAR,	GREEN,	GREEN,	CLEAR },
			{ GREEN,	GREEN,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ GREEN,	CLEAR,	CLEAR,	CLEAR },
			{ GREEN,	GREEN,	CLEAR,	CLEAR },
			{ CLEAR,	GREEN,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}
	}, {
		// PURPLE
		{
			// NORTH
			{ PURPLE,	PURPLE,	PURPLE,	CLEAR },
			{ CLEAR,	PURPLE,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ CLEAR,	PURPLE,	CLEAR,	CLEAR },
			{ PURPLE,	PURPLE,	CLEAR,	CLEAR },
			{ CLEAR,	PURPLE,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// SOUTH
			{ CLEAR,	PURPLE,	CLEAR,	CLEAR },
			{ PURPLE,	PURPLE,	PURPLE,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ PURPLE,	CLEAR,	CLEAR,	CLEAR },
			{ PURPLE,	PURPLE,	CLEAR,	CLEAR },
			{ PURPLE,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}
	}, {
		// RED
		{
			// NORTH
			{ RED,		RED,	CLEAR,	CLEAR },
			{ CLEAR,	RED,	RED,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ CLEAR,	RED,	CLEAR,	CLEAR },
			{ RED,		RED,	CLEAR,	CLEAR },
			{ RED,		CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// SOUTH
			{ RED,		RED,	CLEAR,	CLEAR },
			{ CLEAR,	RED,	RED,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ CLEAR,	RED,	CLEAR,	CLEAR },
			{ RED,		RED,	CLEAR,	CLEAR },
			{ RED,		CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}
	}, {
		// ORANGE
		{
			// NORTH
			{ ORANGE,	ORANGE,	ORANGE,	CLEAR },
			{ ORANGE,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ ORANGE,	ORANGE,	CLEAR,	CLEAR },
			{ CLEAR,	ORANGE,	CLEAR,	CLEAR },
			{ CLEAR,	ORANGE,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR } 
		}, {
			// SOUTH
			{ CLEAR,	CLEAR,	ORANGE,	CLEAR },
			{ ORANGE,	ORANGE,	ORANGE,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ ORANGE,	CLEAR,	CLEAR,	CLEAR },
			{ ORANGE,	CLEAR,	CLEAR,	CLEAR },
			{ ORANGE,	ORANGE,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}
	}, {
		// YELLOW
		{
			// NORTH
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// EAST
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// SOUTH
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}, {
			// WEST
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	YELLOW,	YELLOW,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR },
			{ CLEAR,	CLEAR,	CLEAR,	CLEAR }
		}
	}
};

// Function prototypes
static void p_copynext(void);
static void p_getnext(void);
static bool p_isoffbrd(int x, int y, p_rot_t rot);
static bool p_iscollide(int x, int y, p_rot_t rot);
static void p_copytobd(void);
static void p_drawpiece(SDL_Surface *screen, SDL_Surface *blocks, 
		piece_t *piece);
static bd_col_t p_randcol(void);
static p_rot_t p_randrot(void);

/*
 *	Get the pseudo-random game and next pieces and set their starting 
 *	positions.  srand should've been called.
 */
void
p_init(void) {
	piece.col = p_randcol();
	piece.rot = p_randrot();
	piece.x = P_XORG;
	piece.y = P_YORG;
	p_getnext();
}

/*
 *	Copy the next game piece to the current game piece and move it onto the 
 *	game board.
 */
void
p_copynext(void) {
	piece.col = nextpiece.col;
	piece.rot = nextpiece.rot;
	piece.x = P_XORG;
	piece.y = P_YORG;
}

/*
 *	Get a pseudo-random next piece and set it's starting position.  srand 
 *	should've been called.
 */
void
p_getnext(void) {
	nextpiece.col = p_randcol();
	nextpiece.rot = p_randrot();
	nextpiece.x = P_NEXTX;
	nextpiece.y = P_NEXTY;
}

/*
 *	Move the game piece, in the x direction.  Checks to see if the move would 
 *	take the piece off the game board or collide with a block on the board.  
 *	The move is ignored if it does.
 */
void
p_movex(int x) {
	if (!p_isoffbrd(piece.x + x, piece.y, piece.rot)
	&&  !p_iscollide(piece.x + x, piece.y, piece.rot)) {
		piece.x += x;
	}
}

/*
 *	Move the game piece, in the y direction.  Checks to see if the move would 
 *	take the piece off the game board or collide with a block on the board.  
 *	If it does, copy the piece to the board, move the next piece on to the 
 *	board and get a new next piece.  Returns the number of full lines.
 *	gameover - set to true if the game is over
 */
unsigned
p_movey(int y, bool *gameover) {
	unsigned lines = 0;		// Number of full lines

	assert(gameover != NULL);
	if (p_isoffbrd(piece.x, piece.y + y, piece.rot)
	||  p_iscollide(piece.x, piece.y + y, piece.rot)) {
		p_copytobd();
		lines = bd_chkfull(piece.y, piece.y + P_H - 1);
		p_copynext();
		p_getnext();
		if (p_iscollide(piece.x, piece.y, piece.rot)) {
			*gameover = true;
		}
		if (*gameover) {
			a_play(A_GAMEOVER);
		} else if (lines > 0) {
			a_play(A_LINE);
		}
	} else {
		piece.y += y;
	}
	return lines;
}

/*
 *	Hard drop the piece into place.  Returns the number of full lines.
 *	gameover	- set to true if the game is over
 *	dist		- set to the distance the piece was dropped.
 */
unsigned 
p_harddrop(bool *gameover, unsigned *dist) {
	int i;
	unsigned lines;		// Number of full lines

	assert(gameover != NULL && dist != NULL);
	for (i = 1;
		 !p_isoffbrd(piece.x, piece.y + i, piece.rot)
	  && !p_iscollide(piece.x, piece.y + i, piece.rot);
		 i++) {
		// VOID
	}
	*dist = i - 1;
	piece.y += *dist;
	p_copytobd();
	lines = bd_chkfull(piece.y, piece.y + P_H - 1);
	p_copynext();
	p_getnext();
	if (p_iscollide(piece.x, piece.y, piece.rot)) {
		*gameover = true;
	}
	if (*gameover) {
		a_play(A_GAMEOVER);
	} else if (lines > 0) {
		a_play(A_LINE);
	} else {
		a_play(A_DROP);
	}
	return lines;
}

/*
 *	Rotate the piece.  vel is the velocity and can be 1 or -1.  If the 
 *	rotation takes the piece off the board or collides with a block on the 
 *	board it is ignored.
 */
void
p_rot(int vel) {
	p_rot_t rot;

	assert(vel == -1 || vel == 1);
	if (vel > 0 && piece.rot >= P_ROTS - 1) {
		rot = 0;
	} else if (vel < 0 && piece.rot <= 0) {
		rot = P_ROTS - 1;
	} else {
		rot = piece.rot + vel;
	}
	if (!p_isoffbrd(piece.x, piece.y, rot)
	&&  !p_iscollide(piece.x, piece.y, rot)) {
		piece.rot = rot;
	}
}

/*
 *	Returns true if the piece would be off the edge of the game board using 
 *	the given rotation and position.
 */
bool
p_isoffbrd(int x, int y, p_rot_t rot) {
	bd_col_t col;

	for (int j = 0; j < P_H; j++) {
		for (int i = 0; i < P_W; i++) {
			col = p_blocks[piece.col][rot][j][i];
			if (col != CLEAR) {
				if (bd_isoff(x + i, y + j)) {
					return true;
				}
			}
		}
	}
	return false;
}

/*
 *	Returns true if the piece collides with the game board, using the given 
 *	rotation and position.
 */
bool
p_iscollide(int x, int y, p_rot_t rot) {
	bd_col_t col;

	for (int j = 0; j < P_H; j++) {
		for (int i = 0; i < P_W; i++) {
			col = p_blocks[piece.col][rot][j][i];
			if (col != CLEAR) {
				if (bd_iscollide(x + i, y + j)) {
					return true;
				}
			}
		}
	}
	return false;
}

/*
 *	Copy the piece to board.
 */
void
p_copytobd(void) {
	bd_col_t col;

	for (int j = 0; j < P_H; j++) {
		for (int i = 0; i < P_W; i++) {
			col = p_blocks[piece.col][piece.rot][j][i];
			if (col != CLEAR) {
				bd_copytobd(piece.x + i, piece.y + j, col);
			}
		}
	}
}

/*
 *	Draw the Tetrimino game and next pieces.
 *	screen	- screen surface
 *	blocks	- blocks bitmap
 */
void
p_draw(SDL_Surface *screen, SDL_Surface *blocks) {
	assert(screen != NULL && blocks != NULL);
	p_drawpiece(screen, blocks, &piece);
	p_drawpiece(screen, blocks, &nextpiece);
}

/*
 *	Draw a game piece.
 *	screen	- screen surface
 *	blocks	- blocks bitmap
 *	piece	- the piece to draw
 */
void
p_drawpiece(SDL_Surface *screen, SDL_Surface *blocks, piece_t *piece) {
	bd_col_t col;

	assert(screen != NULL && blocks != NULL && piece != NULL);
	for (int j = 0; j < P_H; j++) {
		for (int i = 0; i < P_W; i++) {
			col = p_blocks[piece->col][piece->rot][j][i];
			if (col != CLEAR) {
				bd_drawblk(screen, blocks, col, piece->x + i, piece->y + j, 
						false);
			}
		}
	}
}

/*
 *	Returns a pseudo-random piece colour.  srand should've been called to set 
 *	the initial seed.
 */
bd_col_t
p_randcol(void) {
	return (bd_col_t) (rand() % (BD_COLS-1)) + 1;
}

/*
 *	Returns a pseudo-random piece rotation.  srand should've been called to 
 *	set the initial seed.
 */
p_rot_t
p_randrot(void) {
	return (p_rot_t) rand() % P_ROTS;
}
