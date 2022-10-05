CPPFLAGS = -MMD
CC = gcc -fsanitize=address
CFLAGS = -Wall -Wextra -std=c99 -g \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 SDL2_image)

SRC = main.c \
	  cutter/cutter.c \
	  preprocessing/preprocessing.c \
	  neural-net/neural-net.c \
	  solver/solver.c \
	  ui/ui.c \
	  utils.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

#don't raise error if DEP files not found
-include ${DEP}
