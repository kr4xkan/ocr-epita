# Makefile

CC = gcc -Ofast
CPPFLAGS =
CFLAGS = -Wall -Wextra `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

all: preprocessing grayscale binarization binarization2 local

SRC = preprocessing.c grayscale.c binarization.c binarization2.c local.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

local: local.o
preprocessing: preprocessing.o
grayscale: grayscale.o
binarization2: binarization2.o
binarization: binarization.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
