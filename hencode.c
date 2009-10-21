#include "safefunctions.h"
#include "treesearch.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define WRITEONLY O_CREAT | O_WRONLY | O_TRUNC
#define DEFPERMS S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

node *create_header(int fdin, int fdout);
void encode(int fdin, int fdout, node *treeroot);

int main(int argc, char *argv[]) {
	int infile, outfile;
	node *treeroot;

	/* Checks to see if output file arg passed */
	if (argc > 2)
		outfile = safe_open(argv[2], WRITEONLY, DEFPERMS);
	/* If no output file specified, use stdout */
	else
		outfile = STDOUT_FILENO;

	/* Checks to see if input file arg passed */
	if (argc > 1)
		infile = safe_open(argv[1], O_RDONLY, DEFPERMS);
	else {
		printf("No file passed!\n");
		exit(1);
	}

	treeroot = create_header(infile, outfile);
	lseek(infile, 0, SEEK_SET);
	encode(infile, outfile, treeroot);

	/* Closes any open files */
	safe_close(infile);
	if (outfile != STDOUT_FILENO)
		safe_close(outfile);

	return 0;
}

node *create_header(int fdin, int fdout) {
	int counts[256];
	int totalchars;
	int i;
	char c;
	node *root;
	
	/* Loads the counter array with zeroes */
	for (i = 0; i < 256; i ++) {
		counts[i] = 0;
	}
	totalchars = 0;

	/* Builds counters with char counts */
	while (safe_read(fdin, &c, sizeof(char)) > 0) {
		counts[(int)c] ++;
		totalchars ++;
	}

	/* Write header to file */
	safe_write(fdout, &totalchars, sizeof(int));
	for (i = 0; i < 256; i ++) {
		if (counts[i] > 0) {
			safe_write(fdout, &i, sizeof(char));
			safe_write(fdout, &counts[i], sizeof(int));
		}
	}
	
	root = build_h_tree(counts, totalchars);
	
	return root;
}

void encode(int fdin, int fdout, node *treeroot) {
	char code = 0;
	int location = 0;
	char c;
	int parentinfo[8];
	int i;

	while (safe_read(fdin, &c, sizeof(char)) > 0) {

		node *thenode = bfs(treeroot, c);

		for (i = 0; i < 8; i ++) {
			parentinfo[i] = -1;
		}
		i = 0;
		while (thenode->parent != NULL) {
			if (thenode->parent->left == thenode)
				parentinfo[i] = 0;
			else
				parentinfo[i] = 1;

			thenode = thenode->parent;
			i ++;
		}

		for (i = 7; i >= 0; i --) {
			if (parentinfo[i] > -1) {
				if (parentinfo[i] == 0)
					code = (code << 1) & 0xfe;
				else
					code = (code << 1) | 0x01;

				/* printf("   %d => 0x%x\n", parentinfo[i], code); */
				location ++;
				if (location > 7) {
					/* printf(" 0x%x\n", code); */
					safe_write(fdout, &code, sizeof(char));
					location = 0;
					code = 0;
				}
			}
		}

	}

	if (location != 0) {
		for (i = location; i < 8; i ++)
			code = (code << 1) & 0xfe;

		safe_write(fdout, &code, sizeof(char));
	}
}

