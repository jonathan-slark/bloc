/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Requires: <SDL/SDL.h>
 *
 *	BMP font definitions.
 */

#ifndef BMPFONT_H
#define BMPFONT_H

#define BF_FONTH	12		// Font height
#define BF_FONTW	12		// Font width
#define BF_ASCMIN	32		// Printable ASCII range
#define BF_ASCMAX	126

// Message box text alignment
typedef enum { BF_LEFT = 1, BF_CENTRE } bf_align_t;

// Function prototypes
extern void bf_msgbox(SDL_Surface *screen, SDL_Surface *font, 
		SDL_Surface *msgbox, bf_align_t align, const char *fmt, ...);
extern void bf_printf(SDL_Surface *screen, SDL_Surface *font, int x, int y, 
		const char *fmt, ...);

#endif // BMPFONT_H
