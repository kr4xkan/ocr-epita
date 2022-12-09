CC = gcc -g 
CFLAGS = -Wall -Og -O3 \
		 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image SDL2_ttf)
LDLIBS = -lm $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image SDL2_ttf)

SRC = ui.c drawarea.c ../utils.c ../preprocessing/preprocessing.c ../cutter/cutter.c ../cutter/crop-manager.c

OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

ui: ${OBJ}

.PHONY: clean

clean:
	${RM} *.png
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} ui

-include ${DEP}

