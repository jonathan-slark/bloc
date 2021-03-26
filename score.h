/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Requirements: <stdbool.h>, <SDL/SDL.h>
 */

#ifndef SCORE_H
#define SCORE_H

#define S_MAXNAME	20	// Maximum player name length

// Game score
typedef unsigned long score_t;

// Function prototypes
extern bool s_entername(SDL_Surface *screen, SDL_Surface *font, 
		SDL_Surface *bg, char *name, unsigned maxname);
extern bool s_display(SDL_Surface *screen, SDL_Surface *font, SDL_Surface *bg, 
		int x, int y);
extern void s_load(void);
extern void s_save(void);
extern void s_newhigh(score_t score, const char *name);
extern void s_award(unsigned lines, unsigned level, unsigned dist, 
		unsigned maxdist);
extern bool s_ishigh(score_t score);
extern void s_init(void);
extern void s_cleanup(void);
extern score_t s_get(void);

#endif // SCORE_H
