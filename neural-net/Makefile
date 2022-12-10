CPPFLAGS = -MMD
CC = gcc -Ofast
CFLAGS = -Wall -Wextra -std=c99 \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 SDL2_image)

SRC = main.c neural-net.c matrix.c ../utils.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

-include ${DEP}
