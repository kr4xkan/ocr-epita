CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDFLAGS = 
LDLIBS = -lm $(shell pkg-config --libs sdl2 SDL2_image)

SRC = cutter.c crop-manager.c ../utils.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

cutter: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} cutter
	${RM} result.png rotated.png
	${RM} squares/*


-include ${DEP}
