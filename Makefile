CC = gcc
CPPFLAGS = -MMD
LDFLAGS =
CFLAGS = -Wall -O3 -std=c99 \
		 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image)
LDLIBS = -lm $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image)

SRC = main.c utils.c \
	  ui/ui.c ui/drawarea.c \
	  preprocessing/preprocessing.c \
	  cutter/cutter.c cutter/crop-manager.c \
	  neural-net/neural-net.c neural-net/matrix.c \
	  solver/solver.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main
	mv main pc2ocr

main: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} pc2ocr

-include ${DEP}
