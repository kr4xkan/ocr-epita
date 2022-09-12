CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = main.c
OBJ = ${SRC:.c=.o}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} main
