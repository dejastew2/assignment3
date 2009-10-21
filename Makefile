CC = gcc

CFLAGS = -Wall -O -pedantic -g

LD = gcc

LDFLAGS = 

hencode: hencode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -o hencode hencode.o safefunctions.o treesearch.o

hdecode: hdecode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o safefunctions.o treesearch.o

safefunctions.o: safefunctions.c
	$(CC) $(CFLAGS) -c -o safefunctions.o safefunctions.c

treesearch.o: treesearch.c
	$(CC) $(CFLAGS) -c -o treesearch.o treesearch.c

hencode.o: hencode.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c

hdecode.o: hdecode.c
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c

test: hencode hdecode
	echo "Testing hencode..."
	./hencode test test2
	echo "Testing hdecode..."
	./hdecode test2
	echo "Done"

clean: hencode.o hdecode.o
	rm -rf hencode.o hdecode.o safefunctions.o treesearch.o

all: hencode.o hdecode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -o hencode hencode.o safefunctions.o treesearch.o
	$(CC) $(CFLAGS) -o hdecode hdecode.o safefunctions.o treesearch.o
