CC = gcc
C_FLAGS = -std=c11
SRC = solution.c
OBJ = $(patsubst %.c,%.o,${SRC})

.PHONY: clean

all: solution test

solution: solution.o
	${CC} -o libsolution.so $< -shared

solution.o: ${SRC}
	${CC} -c $< ${C_FLAGS} -fPIC

test: test.c solution
	${CC} $< -o $@.out -std=c11 -Wl,-rpath . -L . -lsolution

clean:
	@rm -f *.o *.so *.out
