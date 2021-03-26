.POSIX:

VERSION	= 1.1.1

COPT	= -Os -march=native -DNDEBUG
#COPT	= -g -DDEBUG
CFLAGS	= $(COPT) -std=c99 -pedantic-errors -Wall -Wextra -Werror \
		  $(shell sdl-config --cflags)
LDOPT	= -s
#LDOPT	=
LDFLAGS	= $(LDOPT) $(shell sdl-config --libs) -lSDL_image
CC		= gcc

BIN		= bloc
EXE		= $(BIN).exe
OBJ		= audio.o bloc.o bmpfont.o board.o menu.o piece.o score.o
DISTDIR	= $(BIN)_$(VERSION)
DISTZIP	= $(BIN)_$(VERSION)_win.zip
DISTTGZ	= $(BIN)_$(VERSION)_unix.tar.gz
SRCZIP	= $(BIN)_$(VERSION)_source.zip

$(BIN): $(OBJ)
	@$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

audio.o: audio.c audio.h
	@$(CC) $(CFLAGS) -c audio.c

bloc.o: bloc.c audio.h bloc.h bmpfont.h board.h menu.h piece.h score.h
	@$(CC) $(CFLAGS) -c bloc.c

bmpfont.o: bmpfont.c bloc.h bmpfont.h
	@$(CC) $(CFLAGS) -c bmpfont.c

board.o: board.c bloc.h board.h
	@$(CC) $(CFLAGS) -c board.c

menu.o: menu.c bloc.h bmpfont.h menu.h
	@$(CC) $(CFLAGS) -c menu.c

piece.o: piece.c board.h piece.h
	@$(CC) $(CFLAGS) -c piece.c

score.o: score.c bloc.h bmpfont.h score.h
	@$(CC) $(CFLAGS) -c score.c

all: $(BIN)

clean:
	@rm -f $(BIN) $(EXE) $(OBJ)

source:
	@rm -f $(SRCZIP)
	@7z a $(SRCZIP) *.c *.h LICENSE.txt Makefile README.txt

distwin: all
	@rm -f $(DISTZIP)
	@7z a $(DISTZIP) $(EXE) image/*.png sound/*.wav libpng15-15.dll \
		LICENSE.txt LICENSE_png.txt LICENSE_zlib.txt README.txt \
		README_font-pack.txt README_SDL.txt README_SDL_image.txt \
		SDL.dll SDL_image.dll zlib1.dll

distunix:
	@rm -f $(DISTTGZ)
	@mkdir $(DISTDIR) $(DISTDIR)/image $(DISTDIR)/sound
	@cp image/*.png $(DISTDIR)/image
	@cp sound/*.wav $(DISTDIR)/sound
	@cp *.c *.h LICENSE.txt Makefile README.txt README_font-pack.txt $(DISTDIR)
	@tar cf $(DISTTGZ) $(DISTDIR)
	@rm -rf $(DISTDIR)

.PHONY: all clean source distwin distunix
