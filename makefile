CC=gcc
CFLAGS= -std=c99 -Wall -pedantic -g3

all: TSP

TSP: tsp.c location.c location.h
	${CC} ${CFLAGS} -o TSP tsp.c location.c location.h -lm

clean:
	rm -r *.o TSP