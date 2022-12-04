# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -std=c99 -g \
		 $(shell pkg-config --cflags sdl2 SDL2_image)
LDFLAGS =
LDLIBS = -lm $(shell pkg-config --libs sdl2 SDL2_image)

all: preprocessing

SRC = preprocessing.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

preprocessing: preprocessing.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}
	rm *.png

# END
