CC=gcc
CFLAGS= -Wall -m32

all: test

test: main.o memchunk.o

	$(CC) $(CFLAGS) main.o memchunk.o -o test

main.o: main.c memchunk.h
	$(CC) $(CFLAGS) -c main.c

memchunk.o: memchunk.c memchunk.h
	$(CC) $(CFLAGS) -c memchunk.c

clean:
	rm -rf *.o test




