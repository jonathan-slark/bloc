/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Code for the current game score and high score data structures.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "bloc.h"
#include "bmpfont.h"
#include "score.h"

#define S_NUMAWARDS	5				// Number of possible basic awards + 1
#define S_FILE		"scores.txt"	// High scores save file
#define S_READONLY	"r"				// Open file read only
#define S_WRITEONLY	"w"				// Open file to write
#define S_MAXSTR	1024			// Maximum length of line in file
#define S_NUMHIGH	10				// Number of high scores
#define S_HIGHW		11				// Maximum width of high score
#define S_HIGHSPC	24				// Amount of space between high scores
#define S_HIGHX		0				// High score offset
#define S_HIGHY		(B_GAMEH-S_NUMHIGH*BF_FONTH-(S_NUMHIGH-1)*S_HIGHSPC)/2
#define S_TICKLEN	20				// Tick length in ms when entering name

// High score
typedef struct {
	score_t		score;	// Player's score
	char 		*name;	// Player's name
} high_t;

/*
 *	Basic award given for the number of lines cleared at the same time.
 */
static const score_t s_awards[S_NUMAWARDS] = {
	0,		// Unused
	100,	// 1 line
	300,	// 2 lines
	600,	// 3 lines
	1000	// Tetris!
};

// The score for this game
static score_t score = 0;

// High scores table
static high_t s_high[S_NUMHIGH];

// Default high scores, if scores.txt is missing
static const high_t s_defaults[S_NUMHIGH] = {
	// 20 chars "12345678901234567890"
	{ 200000,	"God-like" },
	{ 150000,	"Super-human" },
	{ 100000,	"Olympian" },
	{ 75000,	"Elite" },
	{ 50000,	"Outstanding" },
	{ 25000,	"Exceeds Expectations" },
	{ 10000,	"Acceptable" },
	{ 5000,		"Poor" },
	{ 2500,		"Dreadful" },
	{ 1000,		"Troll" }
};

// Function prototypes
static bool s_keys(bool *done, char *name, unsigned *len, unsigned maxname);
static void s_print(SDL_Surface *screen, SDL_Surface *font, int x, int y);

/*
 *	Ask player to enter name.  We can get typed text by simply using Unicode 
 *	translation in SDL.  Returns true if exiting the game.
 *	screen	- screen surface
 *	font	- font bitmap
 *	bg		- message box background
 *	name	- entered player's name, storage must already be allocated
 *	maxname	- maximum player's name length + 1 for \0
 */
bool
s_entername(SDL_Surface *screen, SDL_Surface *font, SDL_Surface *bg, 
		char *name, unsigned maxname) {
	bool		done	= false;	// Finished entering name if set to true
	bool		exit	= false;	// Exiting program
	unsigned	n		= 0;		// Length of name entered
	Uint32		nexttick;			// Time, in ms, of next tick

	assert(screen != NULL && font != NULL && bg != NULL && name != NULL);
	name[n] = '\0';
	SDL_EnableUNICODE(1);
	nexttick = SDL_GetTicks() + S_TICKLEN;
	do {
		bf_msgbox(screen, font, bg, BF_LEFT, "Enter name: %s", name);
		b_update(screen);
		exit = s_keys(&done, name, &n, maxname);
		SDL_Delay(b_delaylen(nexttick));
		nexttick += S_TICKLEN;
	} while (!done);
	SDL_EnableUNICODE(0);
	return exit;
}

/*
 *	Handle keyboard input for entering player name.  To keep things simple 
 *	we stick to printable ASCII values.  The translated Unicode is simply 
 *	converted into a char value if it's in the range we want.  Backspace is 
 *	supported.  Returns true if exiting the game.
 *	name	- entered player's name, storage must already be allocated
 *	len		- current length of name
 *	maxname	- maximum player's name length + 1 for \0
 */
bool
s_keys(bool *done, char *name, unsigned *len, unsigned maxname) {
	SDL_Event	event;
	bool exit = false;	// Set to true if we're exiting the game

	assert(done != NULL && name != NULL && len != NULL);
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_BACKSPACE:
						if (*len > 0) {
							name[--(*len)] = '\0';
						}
						break;
					case SDLK_ESCAPE:
					case SDLK_RETURN:
						*done = true;
						break;
					default:
						if (*len < maxname
								&& event.key.keysym.unicode >= BF_ASCMIN
								&& event.key.keysym.unicode <= BF_ASCMAX) {
							name[(*len)++] = (char) event.key.keysym.unicode;
							name[*len] = '\0';
						}
						break;
				}
				break;
			case SDL_QUIT:
				*done = exit = true;
				break;
			default:
				// VOID
				break;
		}
	}
	return exit;
}

/*
 *	Display the high scores and wait for key.
 *	screen	- screen surface
 *	font	- font bitmap
 *	bg		- scores background
 */
bool
s_display(SDL_Surface *screen, SDL_Surface *font, SDL_Surface *bg, int x,
		int y) {
	assert(screen != NULL && font != NULL && bg != NULL);
	b_drawbg(screen, bg);
	s_print(screen, font, x + S_HIGHX, y + S_HIGHY);
	b_update(screen);
	return b_waitkey(B_ANYKEY);
}

/*
 *	Load high scores file and read into the table.  If scores.txt is missing 
 *	or incomplete the default high scores are used.  New memory is allocated 
 *	for all of the names in the table, even when using default scores so that 
 *	we don't need to keep track of what needs to be free'd.
 */
void
s_load(void) {
	FILE *fp;				// High score file
	char s[S_MAXSTR];		// Storage for line from high score file
	char *name;				// Player's name
	int i, n;

	fp = fopen(S_FILE, S_READONLY);
	if (fp == NULL) {
		i = 0;
	} else {
		for (i = 0; fgets(s, S_MAXSTR, fp) != NULL; i++) {
			if (i >= S_NUMHIGH) {
				fprintf(stderr, "Error: too many high scores in file %s\n", 
						S_FILE);
				break;
			}
			s_high[i].score = (score_t) strtoul(s, &name, 0);
			if (errno == ERANGE) {
				b_error("Error: high score out of range\n");
			}
			while (isspace(*name)) {
				name++;
			}
			n = strlen(name);
			if (name[n-1] == '\n') {
				name[n-1] = '\0';
			}
			s_high[i].name = b_strdup(name);
		}
	}
	for ( ; i < S_NUMHIGH; i++) {
		s_high[i].score = s_defaults[i].score;
		s_high[i].name = b_strdup(s_defaults[i].name);
	}
	if (fp != NULL) {
		if (ferror(fp)) {
			fprintf(stderr, "Error reading high scores file %s\n", S_FILE);
		}
		if (fclose(fp) == EOF) {
			fprintf(stderr, "Error closing high scores file %s\n", S_FILE);
		}
	}
}

/*
 *	Save high scores to file.
 */
void
s_save(void) {
	FILE *fp;				// High score file

	fp = fopen(S_FILE, S_WRITEONLY);
	if (fp == NULL) {
		fprintf(stderr, "Error opening high scores file %s for writing\n", 
				S_FILE);
		return;
	}
	for (int i = 0; i < S_NUMHIGH; i++) {
		fprintf(fp, "%lu\t%s\n", s_high[i].score, s_high[i].name);
	}
	if (ferror(fp)) {
		fprintf(stderr, "Error writing high scores file %s\n", S_FILE);
	}
	if (fclose(fp) == EOF) {
		fprintf(stderr, "Error closing high scores file %s\n", S_FILE);
	}
}

/*
 *	Enter a new high score into the table, bottom score will "fall" off the 
 *	table.
 *	score	- new high score
 *	name	- player's name
 */
void
s_newhigh(score_t score, const char *name) {
	int i;
	
	assert(name != NULL);
	for (i = 0; i < S_NUMHIGH - 1 && score <= s_high[i].score; i++) {
		// VOID
	}
	assert(score > s_high[i].score);
	free(s_high[S_NUMHIGH-1].name);
	for (int j = S_NUMHIGH - 1; j > i; j--) {
		s_high[j].score = s_high[j-1].score;
		s_high[j].name = s_high[j-1].name;
	}
	s_high[i].score = score;
	s_high[i].name = b_strdup(name);
}

/*
 *	Print the high scores at the position specified.
 *	screen	- screen surface
 *	font	- font bitmap
 */
void 
s_print(SDL_Surface *screen, SDL_Surface *font, int x, int y) {
	assert(screen != NULL && font != NULL);
	for (int i = 0; i < S_NUMHIGH; i++) {
		bf_printf(screen, font, x, y, "%*d %s", S_HIGHW, s_high[i].score, 
				s_high[i].name);
		y += BF_FONTH + S_HIGHSPC;
	}
}

/*
 *	Award score, based on the number of lines cleared, the level and the 
 *	distance dropped, in the case of a hard drop.
 */
void
s_award(unsigned lines, unsigned level, unsigned dist, unsigned maxdist) {
	score_t award;		// Basic award
	score_t bonus;		// Hard drop bonus

	assert(lines < S_NUMAWARDS && dist <= maxdist && maxdist > 0);
	award = s_awards[lines];
	bonus = (score_t) ((double) dist / (double) maxdist * (double) award);
	score += level * (bonus + award);
}

/*
 *	Does our score qualify to be entered on the high score table?
 *	score - score to compare with high scores
 */
bool
s_ishigh(score_t score) {
	return score > s_high[S_NUMHIGH-1].score;
}

/*
 *	Initialise the current game's score.
 */
void
s_init(void) {
	score = 0;
}

/*
 *	Free memory used for player names.
 */
void 
s_cleanup(void) {
	for (int i = 0; i < S_NUMHIGH; i++) {
		free(s_high[i].name);
	}
}

/*
 *	Return the current game's score.
 */
score_t
s_get(void) {
	return score;
}
