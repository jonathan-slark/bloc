/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Main game code, i.e. control loop, SDL code, and other non-data related
 *	functions.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL.h"
#include "SDL_image.h"
#include "audio.h"
#include "bloc.h"
#include "bmpfont.h"
#include "board.h"
#include "menu.h"
#include "piece.h"
#include "score.h"

#define B_WMTITLE		"bloc"			// Window's title
#define B_SCRBPP		0				// 0: current display bits per pixel
#define B_BLKSFILE		"image/blocks.png"	// Block sprites
#define B_TITLEFILE		"image/title.png"	// Title bitmap
#define B_GAMEFILE		"image/game.png"	// Main game background
#define B_ICONFILE		"image/icon.png"	// Window icon
#define B_FONTFILE		"image/font.png"	// Bitmap font
#define B_MENUFILE		"image/menu.png"	// Menu background
#define B_MSGFILE		"image/msg.png"		// Message box background
#define	B_TICKLEN		20				// Game tick length in milliseconds
#define B_MOVETICKS		5				// Number of ticks between movements
#define B_GRAVTICKS		30				// Number of ticks gravity starts at
#define B_DIFFTICKS		3000			// How often difficulty increases
#define B_INFOX			294				// Information display position
#define B_INFOY			534
#define B_INFOW			11				// Information display width, in chars
#define B_MAXNAME		24				// Player's name maximum length
#define B_INTROX		24				// Intro text offset
#define B_INTROY		24

// Current level, based on current difficulty
#define B_LEV(x)		(1 + B_GRAVTICKS - (x))

// Game piece's movement
typedef struct {
	int			xvel;	// x velocity
	unsigned	xticks;	// Number of ticks till next x movement
	unsigned	yticks;	// Number of ticks till next y movement (soft drop)
} b_move_t;

// Game piece's gravity
typedef struct {
	unsigned	diff;		// Current difficulty
	unsigned	dropticks;	// Number of game ticks till next drop
	unsigned	diffticks;	// Number of game ticks till next difficulty
} b_grav_t;

static SDL_Surface	*b_screen	= NULL;	// Game area
static SDL_Surface	*b_title	= NULL;	// Title bitmap
static SDL_Surface	*b_game		= NULL;	// Main game bitmap
static SDL_Surface	*b_blocks	= NULL;	// Blocks bitmap
static SDL_Surface	*b_icon		= NULL;	// Window icon bitmap
static SDL_Surface	*b_font		= NULL;	// Bitmap font
static SDL_Surface	*b_menu		= NULL;	// Menu background
static SDL_Surface	*b_msg		= NULL;	// Message box background

// Function prototypes
static void b_setpal(SDL_Surface *screen, SDL_Surface *bmp);
static void b_drawtitle(SDL_Surface *screen, SDL_Surface *title);
static SDL_Surface *b_loadimage(const char *file);
static void b_cleanup(void);
static bool b_keys(b_move_t *move, b_grav_t *grav, bool *gameover,
		bool *exit);
static void b_move(b_move_t *move, b_grav_t *grav, bool *gameover);
static void b_drawinfo(const b_grav_t *grav);
static void b_init(void);
static void b_setseed(void);

/*
 *	Start a new game.  Returns true if we are exiting the game, i.e. the user
 *	has closed the window.
 */
bool
b_newgame(void) {
	bool		quit		= false;		// Quit once set to true
	bool		exit		= false;		// Exit game when set to true
	bool		gameover	= false;		// Game is over when set to true
	Uint32		nexttick;					// Time, in ms, of next game tick
	b_move_t	move		= { 0, 0, 0 };	// Game piece's movement
	b_grav_t	grav		= {				// Game piece's gravity
		B_GRAVTICKS,
		B_GRAVTICKS,
		B_DIFFTICKS
	};
	char 		name[S_MAXNAME+1];			// Player's name for high score

	s_init();
	p_init();
	bd_init();
	nexttick = SDL_GetTicks() + B_TICKLEN;
	do {
		b_drawbg(b_screen, b_game);
		bd_draw(b_screen, b_blocks);
		p_draw(b_screen, b_blocks);
		b_drawinfo(&grav);
		b_update(b_screen);
		quit = b_keys(&move, &grav, &gameover, &exit);
		b_move(&move, &grav, &gameover);
		bd_chkrm();
		SDL_Delay(b_delaylen(nexttick));
		nexttick += B_TICKLEN;
	} while (!quit && !gameover);
	if (gameover) {
		if (s_ishigh(s_get())) {
			bf_msgbox(b_screen, b_font, b_msg, BF_CENTRE,
					"New high score! Press Return");
			b_update(b_screen);
			exit = b_waitkey(B_RETURNONLY);
			if (!exit) {
				exit = s_entername(b_screen, b_font, b_msg, name, S_MAXNAME);
				s_newhigh(s_get(), name);
			}
		} else {
			bf_msgbox(b_screen, b_font, b_msg, BF_CENTRE,
					"Game over! Press Return");
			b_update(b_screen);
			exit = b_waitkey(B_RETURNONLY);
		}
	}
	return exit;
}

/*
 *	Print introduction and keyboard controls.
 */
bool
b_intro(void) {
	b_drawbg(b_screen, b_menu);
	bf_printf(b_screen, b_font, B_GAMEX+B_INTROX, B_GAMEY+B_INTROY,
		 // "1234567890123456789012345678901234"
			"Fit the shapes together to make   \n"
			"full lines. Full lines are cleared\n"
			"and score points.                 \n"
			"\n"
			"Move the falling shape with the   \n"
			"left and right arrow keys. Rotate \n"
			"the shape with the up arrow key.  \n"
			"\n"
			"Use the space bar to \"hard drop\"\n"
			"the shape into place for extra    \n"
			"points! Use the down arrow key to \n"
			"speed up the falling shape.       \n"
			"\n"
			"The falling shapes gradually speed\n"
			"up but you get bigger scores too! \n"
			"Press escape to quit the game     \n"
			"early. The game ends when the next\n"
			"piece will not fit onto the board.\n"
			"\n"
			"Summary:\n"
			"\n"
			"Up arrow    - rotate\n"
			"Left arrow  - move left\n"
			"Right arrow - move right\n"
			"Down arrow  - soft drop\n"
			"Space bar   - hard drop\n"
			"Escape      - quit\n");
	b_update(b_screen);
	return b_waitkey(B_ANYKEY);
}

/*
 *	Display high scores.
 */
bool
b_scores(void) {
	return s_display(b_screen, b_font, b_menu, B_GAMEX, B_GAMEY);
}

/*
 *	Use the palette from bmp for the screen, if needed.  This assumes all
 *	bitmaps to be used have the same palette and this function is called once.
 *	screen - screen surface
 *	bmp - bitmap to take the palette from
 */
void
b_setpal(SDL_Surface *screen, SDL_Surface *bmp) {
	assert(screen != NULL && bmp != NULL);
	if (bmp->format->palette != NULL && screen->format->palette != NULL) {
		if (!SDL_SetColors(screen, bmp->format->palette->colors, 0,
					bmp->format->palette->ncolors)) {
			b_error("Error setting palette: %s\n", SDL_GetError());
		}
	}
}

/*
 *	Blit title onto screen.
 *	screen - screen surface
 *	title - title bitmap
 */
void
b_drawtitle(SDL_Surface *screen, SDL_Surface *title) {
	assert(screen != NULL && title != NULL);
	if (SDL_BlitSurface(title, NULL, screen, NULL) != 0) {
		b_error("Error blitting title: %s\n", SDL_GetError());
	}
}

/*
 *	Blit game background onto screen.
 *	screen - screen surface
 *	bg - background bitmap
 */
void
b_drawbg(SDL_Surface *screen, SDL_Surface *bg) {
	SDL_Rect dstrect = {
		B_GAMEX,
		B_GAMEY,
		0,	// Unused
		0	// Unused
	};

	assert(screen != NULL && bg != NULL);
	if (SDL_BlitSurface(bg, NULL, screen, &dstrect) != 0) {
		b_error("Error blitting background: %s\n", SDL_GetError());
	}
}

/*
 *	Update game area.
 *	screen - screen surface
 */
void
b_update(SDL_Surface *screen) {
	assert(screen != NULL);
	SDL_UpdateRect(screen, B_GAMEX, B_GAMEY, B_GAMEW, B_GAMEH);
}

/*
 *	Load png file and return pointer to surface.
 */
SDL_Surface *
b_loadimage(const char *file) {
	SDL_Surface *image;

	assert(file != NULL);
	image = IMG_Load(file);
	if (image == NULL) {
		b_error("Error loading %s: %s\n", file, IMG_GetError());
	}
	return image;
}
/*
 *	Duplicate string by allocating new memory and copying the existing string.
 */
char *
b_strdup(const char *s) {
	char *p;

	assert(s != NULL);
	p = malloc(strlen(s) + 1);
	if (p != NULL) {
		strcpy(p, s);
	}
	return p;
}

/*
 *	Return the time, in milliseconds, till next.
 */
Uint32
b_delaylen(Uint32 next) {
	Uint32 now;

	now = SDL_GetTicks();
	if (next <= now) {
		return 0;
	} else {
		return next - now;
	}
}

/*
 *	Prints the formatted error message and exits with status EXIT_FAILURE.
 */
void
b_error(const char *msg, ...) {
	va_list ap;

	assert(msg != NULL);
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	b_cleanup();
	exit(EXIT_FAILURE);
}

/*
 *	Save scores, free memory for scores.  SDL clean-up; free surfaces, if used,
 *	and call SDL_Quit.
 */
void
b_cleanup(void) {
	s_cleanup();
	a_cleanup();
	if (b_msg != NULL) {
		SDL_FreeSurface(b_msg);
	}
	if (b_menu != NULL) {
		SDL_FreeSurface(b_menu);
	}
	if (b_font != NULL) {
		SDL_FreeSurface(b_font);
	}
	if (b_icon != NULL) {
		SDL_FreeSurface(b_icon);
	}
	if (b_blocks != NULL) {
		SDL_FreeSurface(b_blocks);
	}
	if (b_game != NULL) {
		SDL_FreeSurface(b_game);
	}
	if (b_title != NULL) {
		SDL_FreeSurface(b_title);
	}
	IMG_Quit();
	SDL_Quit();
}

/*
 *	Wait for a key press.  If keys equals B_RETURNONLY then only wait for
 *	enter.
 */
bool
b_waitkey(b_waitkey_t keys) {
	SDL_Event	event;
	bool 		quit		= false;		// Quit waiting
	bool 		exit		= false;		// Exit game, i.e. window closed
	Uint32		nexttick;					// Time, in ms, of next game tick

	nexttick = SDL_GetTicks() + B_TICKLEN;
	do {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (keys == B_RETURNONLY) {
						switch (event.key.keysym.sym) {
							case SDLK_RETURN:
							case SDLK_KP_ENTER:
								quit = true;
								break;
							default:
								// VOID
								break;
						}
					} else {
						quit = true;
					}
					break;
				case SDL_QUIT:
					quit = exit = true;
					break;
				default:
					// VOID
					break;
			}
		}
		SDL_Delay(b_delaylen(nexttick));
		nexttick += B_TICKLEN;
	} while (!quit);
	return exit;
}

/*
 *	Handle key presses and user quit request.  User requests quit by closing
 *	the window or pressing escape.  Piece is rotated if user presses up.
 *	Piece is moved left or right if left or right key is pressed.  Piece
 *	should be moved again when the number of ticks has passed, unless the user
 *	releases the key.  Up key is soft drop, space bar is hard drop.
 *	move->xvel		- set to 1 if user is moving piece right, -1 if moving left
 *	move->xticks	- set to number of ticks till next piece movement
 *	move->yticks	- set to number of ticks till next soft drop movement
 *	grav			- required to get current difficulty
 *	gameover		- set to true if the game is over after a hard drop
 *	exit 			- set to true if exiting the game, i.e. window closed
 */
bool
b_keys(b_move_t *move, b_grav_t *grav, bool *gameover, bool *exit) {
	SDL_Event	event;
	bool 		quit		= false;
	unsigned	lines;					// Number of full lines
	unsigned	dist;					// Distance of hard drop

	assert(move != NULL && grav != NULL && gameover != NULL && exit != NULL);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_LEFT:
						move->xvel = -1;
						p_movex(move->xvel);
						move->xticks = B_MOVETICKS;
						break;
					case SDLK_RIGHT:
						move->xvel = 1;
						p_movex(move->xvel);
						move->xticks = B_MOVETICKS;
						break;
					case SDLK_UP:
						p_rot(1);
						break;
					case SDLK_DOWN:
						move->yticks = B_MOVETICKS;
						break;
					case SDLK_SPACE:
						lines = p_harddrop(gameover, &dist);
						if (lines > 0) {
							s_award(lines, B_LEV(grav->diff), dist, BD_H);
						}
						break;
					default:
						// VOID
						break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						if (move->xvel < 0) {
							move->xvel = move->xticks = 0;
						}
						break;
					case SDLK_RIGHT:
						if (move->xvel > 0) {
							move->xvel = move->xticks = 0;
						}
						break;
					case SDLK_DOWN:
						move->yticks = 0;
						break;
					default:
						// VOID
						break;
				}
				break;
			case SDL_QUIT:
				quit = *exit = true;
				break;
			default:
				// VOID
				break;
		}
	}
	return quit;
}

/*
 *	Control piece's movement, both user and gravity.
 *	move->xticks	- countdown is decremented and reset, if necessary
 *	move->yticks	- countdown is decremented and reset, if necessary
 *	grav->diffticks - countdown is decremented and reset, if necessary
 *	grav->diff		- difficulty is increased if gravity countdown hits zero
 *	gameover 		- set to true if gravity results in game over
 */
void
b_move(b_move_t *move, b_grav_t *grav, bool *gameover) {
	unsigned lines;		// Number of full lines

	assert(move != NULL && grav != NULL && gameover != NULL);
	if (move->xticks > 0) {
		if (--move->xticks == 0) {
			p_movex(move->xvel);
			move->xticks = B_MOVETICKS;
		}
	}
	if (move->yticks > 0 && B_MOVETICKS < grav->diff) {
		if (--move->yticks == 0) {
			lines = p_movey(1, gameover);
			if (lines > 0) {
				s_award(lines, B_LEV(grav->diff), 0, BD_H);
			}
			move->yticks = B_MOVETICKS;
		}
	} else if (--grav->dropticks == 0) {
		lines = p_movey(1, gameover);
		if (lines > 0) {
			s_award(lines, B_LEV(grav->diff), 0, BD_H);
		}
		grav->dropticks = grav->diff;
	}
	if (--grav->diffticks == 0) {
		if (grav->diff > 1) {
			grav->diff--;
		}
		grav->diffticks = B_DIFFTICKS;
	}
}

/*
 *	Draw the score and level.
 *	grav - required to get current difficulty
 */
void
b_drawinfo(const b_grav_t *grav) {
	assert(grav != NULL);
	bf_printf(b_screen, b_font, B_INFOX, B_INFOY,
			"Level:\n%*d\n\nScore:\n%*d", B_INFOW, B_LEV(grav->diff),
			B_INFOW, s_get());
}

/*
 *	Initialise SDL, load bmps, set window title and icon, start video, set
 *	initial random seed and load high scores.
 */
void
b_init(void) {
	int flags;	// Flags for SDL init functions

	flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	if (SDL_Init(flags) == -1) {
		b_error("Error initialising SDL: %s\n", SDL_GetError());
	}
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags) {
		b_error("Error initialising SDL_image: %s\n", IMG_GetError());
	}
	b_title = b_loadimage(B_TITLEFILE);
	b_game = b_loadimage(B_GAMEFILE);
	b_blocks = b_loadimage(B_BLKSFILE);
	b_icon = b_loadimage(B_ICONFILE);
	b_font = b_loadimage(B_FONTFILE);
	b_menu = b_loadimage(B_MENUFILE);
	b_msg = b_loadimage(B_MSGFILE);
	SDL_WM_SetCaption(B_WMTITLE, B_WMTITLE);
	SDL_WM_SetIcon(b_icon, NULL);
	b_screen = SDL_SetVideoMode(B_SCRW, B_SCRH, B_SCRBPP,
			SDL_SWSURFACE);
	if (b_screen == NULL) {
		b_error("Error setting video mode: %s\n", SDL_GetError());
	}
	b_setpal(b_screen, b_title);
	a_init();
	b_drawtitle(b_screen, b_title);
	SDL_UpdateRect(b_screen, 0, 0, B_TITLEW, B_TITLEH);
	b_setseed();
	s_load();
}

/*
 *	Set the initial seed by getting the current time.
 */
void
b_setseed(void) {
	srand((unsigned int) time(NULL));
}

/*
 *	Main.
 */
int
main(int argc, char *argv[]) {
	if (argc > 1) {
		fprintf(stderr, "Error: %s takes no parameters\n", argv[0]);
	}
	b_init();
	m_display(b_screen, b_menu, b_font, b_blocks, B_GAMEX, B_GAMEY);
	s_save();
	b_cleanup();
	exit(EXIT_SUCCESS);
}
