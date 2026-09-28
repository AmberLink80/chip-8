CC=gcc
CFLAGS=-I.
DEPS = chip-8.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: chip-8.o
	$(CC) -o chip-8 main.c chip-8.o -lSDL2

clean:
	rm chip-8
	rm chip-8.o

