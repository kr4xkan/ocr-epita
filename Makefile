CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 SDL2_image)

SRC = main.c \
	  cutter/cutter.c \
	  utils.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

SRC_ROT = rotate.c cutter/cutter.c utils.c
OBJ_ROT = ${SRC_ROT:.c=.o}
DEP_ROT = ${SRC_ROT:.c=.d}

all: main rotate

rotate: ${OBJ_ROT}

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main
	${RM} ${OBJ_ROT}
	${RM} ${DEP_ROT}
	${RM} rotate

#don't raise error if DEP files not found
-include ${DEP}
