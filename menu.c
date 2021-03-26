/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Code to a display an in-game menu from the data structure.
 */

#include <assert.h>
#include <stdbool.h>
#include "SDL.h"
#include "bloc.h"
#include "bmpfont.h"
#include "menu.h"

#define M_NUMMAIN	4			// Number of menu items in the main menu
#define M_ITEMSPC	24			// Amount of space between items in menu
#define M_TICKLEN	20			// Menu tick length in miliseconds
#define M_MAINX		100			// Main menu offset from game area origin
#define M_MAINY		204
#define M_CURSW		24			// Cursor dimensions
#define M_CURSH		24
#define M_CURSX		M_MAINX-M_CURSW-12	// Cursor offset from game area
#define M_CURSY		M_MAINY-6
#define M_CURSSPC	12
#define M_CURSSRCX	96			// Cursor position in the blocks bitmap
#define M_CURSSRCY	0

// Menu item
typedef struct {
	bool (*fp)(void);	// Function pointer
	const char *s;		// Menu string
} m_item_t;

// Main menu function prototypes
static bool m_exit(void);

// Main menu
static struct {
	unsigned	cur;				// Currently selected item
	m_item_t	items[M_NUMMAIN];	// List of menu items
} m_main = {
	0,
	{
		{ b_newgame,	"New game" },
		{ b_intro,		"Instructions" },
		{ b_scores,		"High scores" },
		{ m_exit,		"Exit" }
	}
};

// Function prototypes
static bool m_keys(void);
static void m_print(SDL_Surface *screen, SDL_Surface *font, int x, int y);
static void m_cursor(SDL_Surface *screen, SDL_Surface *blocks, int x, int y);

/*
 *	Display the main menu and handle user input.
 *	screen	- screen surface
 *	bg		- menu background
 *	font	- font bitmap
 *	blocks	- blocks bitmap, used for cursor
 */
void
m_display(SDL_Surface *screen, SDL_Surface *bg, SDL_Surface *font, 
		SDL_Surface *blocks, int x, int y) {
	bool 	quit 		= false;	// Set to true if exiting the game
	Uint32	nexttick;				// Time, in ms, of next menu tick

	assert(screen != NULL && bg != NULL && font != NULL && blocks != NULL);
	nexttick = SDL_GetTicks() + M_TICKLEN;
	do {
		b_drawbg(screen, bg);
		m_print(screen, font, x + M_MAINX, y + M_MAINY);
		m_cursor(screen, blocks, x + M_CURSX, y + M_CURSY);
		b_update(screen);
		quit = m_keys();
		SDL_Delay(b_delaylen(nexttick));
		nexttick += M_TICKLEN;
	} while (!quit);
}

/*
 *	Process keys and user quit for menu.  User can choose a menu by using the 
 *	up and down arrow keys.  A menu is selected by pressing return, escape 
 *	quits the game.
 */
bool
m_keys(void) {
	SDL_Event	event;
	bool		exit		= false;	// Exit the game

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						exit = true;
						break;
					case SDLK_UP:
						if (m_main.cur == 0) {
							m_main.cur = M_NUMMAIN - 1;
						} else {
							m_main.cur--;
						}
						break;
					case SDLK_DOWN:
						if (m_main.cur >= M_NUMMAIN - 1) {
							m_main.cur = 0;
						} else {
							m_main.cur++;
						}
						break;
					case SDLK_RETURN:
						exit = (*m_main.items[m_main.cur].fp)();
						break;
					default:
						// VOID
						break;
				}
				break;
			case SDL_QUIT:
				exit = true;
				break;
			default:
				// VOID
				break;
		}
	}
	return exit;
}

/*
 *	Print the main menu items.
 *	screen	- screen surface
 *	font	- font bitmap
 */
void
m_print(SDL_Surface *screen, SDL_Surface *font, int x, int y) {
	assert(screen != NULL && font != NULL);
	for (int i = 0; i < M_NUMMAIN; i++) {
		bf_printf(screen, font, x, y, "%s", m_main.items[i].s);
		y += BF_FONTH + M_ITEMSPC;
	}
}

/*
 *	Draw the cursor.
 *	screen	- screen surface
 *	blocks	- blocks bitmap
 */
void
m_cursor(SDL_Surface *screen, SDL_Surface *blocks, int x, int y) {
	SDL_Rect srcrect = {
		(Sint16) M_CURSSRCX,
		(Sint16) M_CURSSRCY,
		(Uint16) M_CURSW,
		(Uint16) M_CURSH
	};
	SDL_Rect dstrect = {
		(Sint16) x,
		(Sint16) y + m_main.cur * (M_CURSH+M_CURSSPC),
		0,	// Unused
		0	// Unused
	};

	assert(screen != NULL && blocks != NULL);
	if (SDL_BlitSurface(blocks, &srcrect, screen, &dstrect) != 0) {
		b_error("Error blitting cursor: %s\n", SDL_GetError());
	}
}

/*
 *	Exit game.
 */
bool
m_exit(void) {
	return true;
}
