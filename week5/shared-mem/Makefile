CC=gcc
C_FLAGS=-std=c11 -D _XOPEN_SOURCE
OBJ=$(patsubst %.c,%.o,$(wildcard *.c))

.PHONY: all clean

all: ${OBJ}
	${CC} -o solution $?

%.o: %.c
	${CC} -c $< ${C_FLAGS}

clean:
	@rm -f *.o
