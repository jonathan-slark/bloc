/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Display a BMP font.
 */

#include <assert.h>
#include <stdbool.h>
#include "SDL.h"
#include "bloc.h"
#include "bmpfont.h"

#define BF_FONTSPC	6			// Number of pixels between lines
#define BF_ISVERT	true		// Vertical font if true, horizontal otherwise
#define BF_MAXSTR	1024		// Max string length
#define BF_MSGBGW	B_SCRW		// Message box bg dimensions
#define BF_MSGBGH	96
#define BF_MSGBGX	0			// Message box background position
#define BF_MSGBGY	B_GAMEY + (B_GAMEH - BF_MSGBGH) / 2
#define BF_MSGX		24 + BF_FONTW	// Message text position
#define BF_MSGY		B_GAMEY + (B_GAMEH - BF_FONTH) / 2

// Function prototypes
static void bf_print(SDL_Surface *screen, SDL_Surface *font, int x, int y, 
		const char *s);
static void bf_putchar(SDL_Surface *screen, SDL_Surface *font, int x, int y, 
		char c);

/*
 *	Show a message in the middle of the screen in a box.
 *	screen	- screen surface
 *	font	- font bitmap
 *	msgbox	- message box background
 */
void
bf_msgbox(SDL_Surface *screen, SDL_Surface *font, SDL_Surface *msgbox, 
		bf_align_t align, const char *fmt, ...) {
	SDL_Rect dstrect = {
		(Sint16) BF_MSGBGX,
		(Sint16) BF_MSGBGY,
		0,	// Unused
		0	// Unused
	};
	int x;
	va_list ap;
	char s[BF_MAXSTR];

	assert(screen != NULL && font != NULL && msgbox != NULL && fmt != NULL);
	if (SDL_BlitSurface(msgbox, NULL, screen, &dstrect) != 0) {
		b_error("Error blitting block: %s\n", SDL_GetError());
	}
	va_start(ap, fmt);
	vsnprintf(s, BF_MAXSTR, fmt, ap);
	va_end(ap);
	if (align == BF_CENTRE) {
		x = (BF_MSGBGW - strlen(s) * BF_FONTW) / 2;
	} else {
		x = BF_MSGX;
	}
	bf_print(screen, font, x, BF_MSGY, s);
}

/*
 *	Print an ascii string, with formating, on screen at the given position.
 *	screen	- screen surface
 *	font	- bitmap font
 */
void
bf_printf(SDL_Surface *screen, SDL_Surface *font, int x, int y, 
		const char *fmt, ...) {
	va_list ap;
	char s[BF_MAXSTR];

	assert(screen != NULL && font != NULL && fmt != NULL);
	va_start(ap, fmt);
	vsnprintf(s, BF_MAXSTR, fmt, ap);
	va_end(ap);
	bf_print(screen, font, x, y, s);
}

/*
 *	Print an ascii string on screen at the given position.  Supports newline 
 *	character.
 *	screen	- screen surface
 *	font	- bitmap font
 */
void
bf_print(SDL_Surface *screen, SDL_Surface *font, int x, int y, const char *s) {
	int xoff = 0;	// X offset
	int yoff = 0;	// Y offset

	assert(screen != NULL && font != NULL && s != NULL);
	for ( ; *s != '\0'; s++) {
		if (*s == '\n') {
			yoff += BF_FONTH + BF_FONTSPC;
			xoff = 0;
		} else {
			bf_putchar(screen, font, x + xoff, y + yoff, *s);
			xoff += BF_FONTW;
		}
	}
}

/*
 *	Blit a bitmap char to screen.  Uses the actual ascii value to get the 
 *	bitmap from the font surface.
 *	screen	- screen surface
 *	font	- bitmap font
 *	c		- ascii value to print, non-printable chars are ignored
 */
void
bf_putchar(SDL_Surface *screen, SDL_Surface *font, int x, int y, char c) {
	SDL_Rect srcrect = {
		(Sint16) (BF_ISVERT) ? 0 : BF_FONTW * (c - BF_ASCMIN),
		(Sint16) (BF_ISVERT) ? BF_FONTH * (c - BF_ASCMIN) : 0,
		(Uint16) BF_FONTW,
		(Uint16) BF_FONTH
	};
	SDL_Rect dstrect = {
		(Sint16) x,
		(Sint16) y,
		0,	// Unused
		0	// Unused
	};

	assert(screen != NULL && font != NULL);
	if (c < BF_ASCMIN || c > BF_ASCMAX) {
		return;
	}
	if (SDL_BlitSurface(font, &srcrect, screen, &dstrect) != 0) {
		b_error("Error blitting block: %s\n", SDL_GetError());
	}
}
