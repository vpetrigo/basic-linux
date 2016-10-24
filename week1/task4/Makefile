CC = gcc
C_FLAGS = -std=c11
SRC = solution.c
OBJ = $(patsubst %.c,%.o,${SRC})

.PHONY: clean

all: solution test

solution: solution.o
	${CC} -c $(patsubst %.o,%.c,$<) ${C_FLAGS} -fPIC
	${CC} -o libsolution.so $< -shared

test: test.c
	${CC} $< -o $@.out -std=c11 -Wl,-rpath . -L . -lsolution

clean:
	@rm -f *.o *.so *.out