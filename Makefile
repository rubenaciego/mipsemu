
NAME = mipsemu
CC = gcc
CFLAGS = -I.

all:
	${CC} ${CFLAGS} *.c -o build/${NAME}
	
clean:
	rm build/${NAME}
