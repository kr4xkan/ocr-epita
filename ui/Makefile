# Makefile

CC = gcc
CFLAGS = -Wall -O3 \
		 $(shell pkg-config --cflags gtk+-3.0 sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs gtk+-3.0 sdl2 SDL2_image)

EXE = plain disc state paddles duel

all: $(EXE)

$(foreach f, $(EXE), $(eval $(f):))

.PHONY: clean

clean:
	${RM} $(EXE)

# END
