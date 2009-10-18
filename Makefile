CC = gcc

CFLAGS = -Wall -O -ansi -pedantic

LD = gcc

LDFLAGS = 

hencode: hencode.o safefunctions.o
	$(CC) $(CFLAGS) -o hencode hencode.o safefunctions.o

safefunctions.o: safefunctions.c
	$(CC) $(CFLAGS) -c -o safefunctions.o safefunctions.c

hencode.o: hencode.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c

debug: hencode.o safefunctions.o
	$(CC) $(CFLAGS) -g -o hencode hencode.o safefunctions.o
	gdb hencode

test: hencode
	echo "Testing hencode..."
	./hencode test
	echo "Done"

clean: hencode.o safefunctions.o
	rm -rf hencode.o safefunctions.o

all: hencode.o safefunctions.o
	$(CC) $(FLAGS) -o hencode hencode.o safefunctions.o
