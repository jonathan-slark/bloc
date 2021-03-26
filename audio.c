/*
 *	See LICENSE.txt file for copyright and license details.
 *
 *	Audio sub-system.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "SDL.h"
#include "audio.h"

#define A_NUMSOUNDS	3

// Wav data
typedef struct {
	SDL_AudioSpec	spec;
	Uint8			*data;		// Wav's data
	Uint32			datalen;	// Length of wav's data
} a_wav_t;

// Sounds
typedef struct {
	a_sound_t	current;	// Current sound
	Uint32		datapos;	// Current play position
	a_wav_t		wavs[A_NUMSOUNDS];
} a_sounds_t;

// List of sound files to load
static const char *a_files[A_NUMSOUNDS] = {
	"sound/drop.wav",		// Hard drop
	"sound/line.wav",		// Full line
	"sound/game-over.wav"	// Game over
};

static bool			isaudio = true;	// Is audio available?
static a_sounds_t	a_sounds;

// Function prototypes
static void SDLCALL a_callback(void *unused, Uint8 *stream, int len);

/*
 *	Call-back function to play sounds in another thread.
 *	stream:	pointer to the audio buffer to be filled
 *	len:	length (in bytes) of the audio buffer
 */
void SDLCALL 
a_callback(void *unused, Uint8 *stream, int len) {
	// Pointer to current data
	Uint8 *dp = a_sounds.wavs[a_sounds.current].data + a_sounds.datapos;
	// Length of data remaining
	int left = (int) (a_sounds.wavs[a_sounds.current].datalen 
			- a_sounds.datapos);	

	assert(stream != NULL && len > 0);
	// Avoid unused variable warning
	if (unused == unused) {
		// VOID
	}
	if (a_sounds.datapos >= a_sounds.wavs[a_sounds.current].datalen) {
		return;
	}
	len = (len > left) ? left : len;
	SDL_memcpy(stream, dp, len);
	a_sounds.datapos += len;
}

/*
 *	Start playback, there is no mixing, assumes only one sound playing at a 
 *	time.
 */
void
a_play(a_sound_t sound) {
	if (isaudio) {
		SDL_PauseAudio(0);
		a_sounds.current = sound;
		a_sounds.datapos = 0;
	}
}

/*
 *	Initialise the audio sub-system: load wavs and open audio device.
 */
void
a_init(void) {
	for (int i = 0; i < A_NUMSOUNDS; i++) {
		if (SDL_LoadWAV(a_files[i], &a_sounds.wavs[i].spec, 
					&a_sounds.wavs[i].data, 
					&a_sounds.wavs[i].datalen) 
				== NULL) {
			fprintf(stderr, "Error loading sound file %s: %s\n", a_files[i], 
					SDL_GetError());
			isaudio = false;
		}
	}
	a_sounds.wavs[A_DROP].spec.callback = a_callback;
	if (SDL_OpenAudio(&a_sounds.wavs[A_DROP].spec, NULL) != 0) {
		fprintf(stderr, "Error opening audio device: %s\n", SDL_GetError());
		isaudio = false;
	}
}

/*
 *	Cleanup audio sub-system: free wav data and close audio device.
 */
void
a_cleanup(void) {
	SDL_CloseAudio();
	for (int i = 0; i < A_NUMSOUNDS; i++) {
		SDL_FreeWAV(a_sounds.wavs[i].data);
	}
}
