# bloc

**Written by Jonathan Slark**  
See `LICENSE.txt` file for copyright and license details.

## Introduction

`bloc` is based on a well-known game. This version sticks to fairly standard gameplay, but the scoring system is my own. The game was created as an exercise to learn games programming and is not meant to pose as an official version.

### Quick Start Guide

- **Up arrow** - Rotate
- **Left arrow** - Move left
- **Right arrow** - Move right
- **Down arrow** - Soft drop
- **Space bar** - Hard drop
- **Escape** - Quit

For more information on the gameplay, select the *Instructions* item in the game's menu. To navigate the menus, use the arrow keys and select an item with the *Return* key.

### Running the Game

To start `bloc` on Windows, extract all the files to a directory and run `bloc.exe`.

## Notes

- If you want to reset the high scores, simply delete `scores.txt`.
- If the game doesn't start or stops unexpectedly, check if `stderr.txt` exists—it may contain an error message.

## Compiling on Unix-like Systems

### Requirements

- SDL 1.2 + development files
- SDL_image 1.2 + development files
- gcc
- make

### Build Instructions

1. Unpack the tarball.
2. Run:
   ```sh
   make
   ./bloc
   ```

## Changes

### Version 1.1.1
- Small changes for compiling on Unix-like systems.

### Version 1.1
- Using SDL_image library to load PNG files instead of BMP files.
- Added sound effects.

## Credits

- The game links against SDL. See `README_SDL.txt` for more information and license.
- The game links against SDL_image. See `README_SDL_image.txt` for license.
- The in-game font used is from font-pack. See `README-font-pack.txt` for more details, including license and restrictions.
- Sound effects used are from [Sound Jay](http://www.soundjay.com/tos.html).
