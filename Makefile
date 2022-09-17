CPPFLAGS = -MMD
CC = gcc -fsanitize=address
CFLAGS = -Wall -Wextra -std=c99 -O2

SRC = main.c \
	  cutter/cutter.c \
	  preprocessing/preprocessing.c \
	  neural-net/neural-net.c \
	  solver/solver.c \
	  ui/ui.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

-include ${DEP}
