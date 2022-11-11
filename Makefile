CPPFLAGS = -MMD
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 SDL2_image)

SRC = main.c \
	  utils.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main neural preprocessing solver cutter

main: ${OBJ}

neural:
	$(MAKE) -C neural-net/

preprocessing:
	$(MAKE) -C preprocessing/

solver:
	$(MAKE) -C solver/

cutter:
	$(MAKE) -C cutter/

.PHONY: clean preprocessing solver cutter

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

	$(MAKE) -C neural-net/ clean
	$(MAKE) -C preprocessing/ clean
	$(MAKE) -C solver/ clean
	$(MAKE) -C cutter/ clean

#don't raise error if DEP files not found
-include ${DEP}
