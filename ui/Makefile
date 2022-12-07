# Makefile

CC = gcc -fsanitize=address -g
CFLAGS = -Wall -O3 \
		 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image SDL2_ttf)
LDLIBS = -lm $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image SDL2_ttf)

SRC = ui.c drawarea.c \
	  ../utils.c \
	  ../cutter/cutter.c ../cutter/crop-manager.c \
	  ../neural-net/neural-net.c ../neural-net/matrix.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

ui: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} ui

-include ${DEP}
