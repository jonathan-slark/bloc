[]     []
[][][] [] [][][] [][]
[]  [] [] []  [] []
[][][] [] [][][] [][]

Written by Jonathan Slark <bloc@crunchy101.fastmail.fm>, see LICENSE.txt file
for copyright and license details.


Introduction
------------

bloc is based on Tetris.  This version sticks to fairly standard gameplay [1]
but the scoring system is my own.

Quick start guide:

Up arrow	- rotate
Left arrow	- move left
Right arrow	- move right
Down arrow	- soft drop
Space bar	- hard drop
Escape		- quit

For more information on the gameplay select the Instructions item in the game's
menu.  To navigate the menus use the arrow keys and select an item with the
return key.

To start bloc on Windows extract all the files to a directory and run bloc.exe.


Notes
-----

* If you want to reset the high scores, simply delete scores.txt.
* If the game doesn't start or stops unexpectedly check to see if stderr.txt
  exists, it may contain an error message.


Compiling on Unix-like systems
------------------------------

Requirements:
SDL 1.2 + development files
SDL_image 1.2 + development files
gcc
make

Unpack the tarball and run:
make
./bloc


Changes
-------

Version 1.1.1:
* Small changes for compiling on Unix-like systems.

Version 1.1:
* Using SDL_image library to load PNG files, instead of BMP files.
* Added sound effects.


Credits
-------

* The game links against SDL:
  http://www.libsdl.org/
* The game links against SDL_image:
  http://www.libsdl.org/projects/SDL_image/
* The in-game font used is from font-pack, see README-font-pack.txt for more
  details, including license and restrictions.
* Sound effects used are from Sound Jay, for terms of use see:
  http://www.soundjay.com/tos.html


References
----------

[1] https://en.wikipedia.org/wiki/Tetris
