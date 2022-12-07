CPPFLAGS = -MMD
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra -fsanitize=address -std=c99 -g \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = `pkg-config --libs gtk+-3.0` -lm $(shell pkg-config --libs sdl2 SDL2_image)
LDFLAGS = -fsanitize=address

SRC = ui ../cutter/cutter.c ../cutter/crop-manager.c ../utils.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

ui: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} ui


-include ${DEP}
