# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

all: preprocessing grayscale binarization binarization2

SRC = preprocessing.c grayscale.c binarization.c binarization2.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

preprocessing: preprocessing.o
grayscale: grayscale.o
binarization2: binarization2.o
binarization: binarization.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
