# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

all: preprocessing grayscale binarization

SRC = preprocessing.c grayscale.c binarization.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

preprocessing: preprocessing.o
grayscale: grayscale.o
binarization: binarization.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}
	rm out.png

# END
