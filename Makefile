CC = gcc

CFLAGS = -Wall -O -pedantic

LD = gcc

LDFLAGS = 

hencode: hencode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -o hencode hencode.o safefunctions.o treesearch.o

hdecode: hdecode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o safefunctions.o treesearch.o

safefunctions.o: safefunctions.c
	$(CC) $(FLAGS) -c -o safefunctions.o safefunctions.c

treesearch.o: treesearch.c
	$(CC) $(FLAGS) -c -o treesearch.o treesearch.c

hencode.o: hencode.c
	$(CC) $(FLAGS) -c -o hencode.o hencode.c

hdecode.o: hdecode.c
	$(CC) $(FLAGS) -c -o hdecode.o hdecode.c

debug: hdecode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -g -o hencode hencode.o safefunctions.o treesearch.o
	gdb hencode

test: hencode hdecode
	echo "Testing hencode..."
	./hencode test test2
	echo "Testing hdecode..."
	./hdecode test2
	echo "Done"

clean: hencode.o hdecode.o
	rm -rf hencode.o hdecode.o safefunctions.o treesearch.o

all: hencode.o hdecode.o safefunctions.o treesearch.o
	$(CC) $(FLAGS) -o hencode hencode.o safefunctions.o treesearch.o
	$(CC) $(FLAGS) -o hdecode hdecode.o safefunctions.o treesearch.o
