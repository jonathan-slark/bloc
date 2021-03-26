/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Requires <stdbool.h>, <SDL/SDL.h>
 *
 *	Contains main game and general definitions.
 */

#ifndef BLOC_H
#define BLOC_H

#define B_SCRW		456			// Screen size in pixels
#define B_SCRH		648
#define B_TITLEW	B_SCRW		// Title size
#define B_TITLEH	120
#define B_GAMEX		0			// Game display position
#define B_GAMEY		B_TITLEH
#define B_GAMEW		B_SCRW		// Game display size
#define B_GAMEH		B_SCRH - B_GAMEY

#define MIN(x, y)	(((x) < (y)) ? (x) : (y))

// Which keys to wait for?
typedef enum { B_RETURNONLY = 1, B_ANYKEY } b_waitkey_t;

// Function prototypes
extern bool b_newgame(void);
extern bool b_intro(void);
extern bool b_scores(void);
extern void b_drawbg(SDL_Surface *screen, SDL_Surface *bg);
extern void b_update(SDL_Surface *screen);
extern char *b_strdup(const char *s);
extern Uint32 b_delaylen(Uint32 next);
extern void b_error(const char *msg, ...);
extern bool b_waitkey(b_waitkey_t keys);

#endif // BLOC_H
