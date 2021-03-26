/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Requires: <SDL/SDL.h>
 *
 *	Definitions for the current and next game pieces.
 */

#ifndef PIECE_H
#define PIECE_H

#define P_ROTS	4		// Number of rotations

// Rotations
typedef enum { NORTH = 0, EAST, SOUTH, WEST } p_rot_t;

// Function prototypes
extern void p_init(void);
extern void p_movex(int x);
extern unsigned p_movey(int y, bool *gameover);
extern unsigned p_harddrop(bool *gameover, unsigned *dist);
extern void p_rot(int vel);
extern void p_draw(SDL_Surface *screen, SDL_Surface *blocks);

#endif // PIECE_H
