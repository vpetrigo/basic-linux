CC=gcc
C_FLAGS=-std=c11
OBJ=$(patsubst %.c,%.o,$(wildcard *.c))

all: ${OBJ}
	${CC} -o solution $?

%.o: %.c
	${CC} -c $< ${C_FLAGS}

clean:
	@rm -f *.o
