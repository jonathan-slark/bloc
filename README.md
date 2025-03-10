# bloc

**Author: Jonathan Slark**
See `LICENSE.txt` for copyright and licensing details.

## Introduction

`bloc` is a falling-block puzzle game inspired by a well-known classic. While the core gameplay remains familiar, it features a custom scoring system. This game was developed as a learning project in game programming and is not intended as an official version.

### Quick Start Guide

- **Up arrow** - Rotate
- **Left arrow** - Move left
- **Right arrow** - Move right
- **Down arrow** - Soft drop
- **Space bar** - Hard drop
- **Escape** - Quit

For detailedd gameplay instructions, select *Instructions* from the in-game menu. Navigate the menus using the arrow keys and confirm slections with *Return*.

### Running the Game

## Windows

1. Extract all files to a directory.
2. Run `bloc.exe`.

## Unix-like Systems

### Requirements

- SDL 1.2 + development files
- SDL_image 1.2 + development files
- GCC
- Make

### Build Instructions

1. Extract the tarball.
2. Open a terminal and run:
   ```sh
   make
   ./bloc
   ```

## Additional Notes

- To reset high scores, delete `scores.txt`.
- If the game fails to start or crashes, check `stderr.txt` for error messages.

## Changes

### Version 1.1.1
- Minor changes for compatibility with Unix-like systems.

### Version 1.1
- Switched to SDL_image for PNG file support (replacing BMP).
- Added sound effects.

## Credits

- SDL & SDL_image – See `README_SDL.txt` and `README_SDL_image.txt` for details and licensing.
- Font – Included from font-pack; see `README-font-pack.txt` for licensing details.
- Sound effects - Sourced from [Sound Jay](http://www.soundjay.com/tos.html).
