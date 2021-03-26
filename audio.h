/*
 *	See LICENSE.txt file for copyright and license details.
 */

#ifndef AUDIO_H
#define AUDIO_H

// Sounds available
typedef enum { A_DROP = 0, A_LINE, A_GAMEOVER } a_sound_t;

// Function prototypes
extern void a_play(a_sound_t sound);
extern void a_init(void);
extern void a_cleanup(void);

#endif // AUDIO_H
