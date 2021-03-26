/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Requires: <SDL/SDL.h>
 *
 *	Definitions for the in-game menu.
 */

#ifndef MENU_H
#define MENU_H

// Function prototypes
extern void m_display(SDL_Surface *screen, SDL_Surface *bg, SDL_Surface *font, 
		SDL_Surface *blocks, int x, int y);

#endif // MENU_H
