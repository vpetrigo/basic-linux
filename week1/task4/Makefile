CC = gcc
C_FLAGS = -std=c11 -fPIC
SRC = solution.c
OBJ = $(patsubst %.c,%.o,${SRC})

.PHONY: clean

all: solution.o
	${CC} -shared -o libsolution.so $<

solution.o: solution.c
	${CC} -c $< ${C_FLAGS}

clean:
	@rm *.o *.so