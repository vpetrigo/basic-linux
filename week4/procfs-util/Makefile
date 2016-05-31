CC=gcc
OBJ=$(patsubst *.c,*.o,$(wildcard *.c))
C_FLAGS=-std=c11

all: ${OBJ}
	${CC} -o solution $?

%.o: %.c
	${CC} -c $< ${C_FLAGS}
