CC = gcc

CFLAGS = -Wall -O -pedantic

LD = gcc

LDFLAGS = 

hencode: hencode.c
	$(CC) $(CFLAGS) -o hencode hencode.c

debug: hencode.c
	$(CC) $(CFLAGS) -g -o hencode hencode.c
	gdb hencode

test: hencode
	echo "Testing hencode..."
	./hencode test
	echo "Done"

clean: hencode.o
	rm -rf hencode.o

all: hencode.o
	$(CC) $(FLAGS) -o hencode hencode.o
