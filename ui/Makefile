# Makefile

CC = gcc
CFLAGS = -Wall -Og -O3 \
		 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image)

EXE = ui

all: $(EXE)

.PHONY: clean

clean:
	${RM} $(EXE)

# END
