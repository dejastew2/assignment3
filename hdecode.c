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
#define BLOCKSIZE 4096

node *read_header(int fdin, int fdout);
void decode(int fdin, int fdout, node *treeroot);

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
		if (argv[1][0] != '-')
			infile = safe_open(argv[1], O_RDONLY, DEFPERMS);
		else
			infile = STDIN_FILENO;
	else {
		infile = STDIN_FILENO;
	}

	treeroot = read_header(infile, outfile);
	decode(infile, outfile, treeroot);

	/* Closes any open files */
	if (infile != STDIN_FILENO)
		safe_close(infile);
	if (outfile != STDOUT_FILENO)
		safe_close(outfile);

	return 0;
}

node *read_header(int fdin, int fdout) {
	int counts[256];
	int totalchars, temptcs;
	int i, tempcount;
	char c;
	node *root;

	/* Loads the counter array with zeroes */
	for (i = 0; i < 256; i ++) {
		counts[i] = 0;
	}
	totalchars = 0;

	safe_read(fdin, &temptcs, sizeof(int));

	while (temptcs > 0) {
		safe_read(fdin, &c, sizeof(char));
		safe_read(fdin, &tempcount, sizeof(int));
		counts[(int)c] = tempcount;
		totalchars += tempcount;
		temptcs -= 1;
	}

	root = build_h_tree(counts, totalchars);

	return root;
}

void decode(int fdin, int fdout, node *treeroot) {
	char c[BLOCKSIZE];
	char o[BLOCKSIZE];
	int read_size;
	int cur_write_size = 0;
	int i, j;

	node *thenode = treeroot;

	while ((read_size = safe_read(fdin, &c, BLOCKSIZE)) > 0) {

		for (i = 0; i < read_size; i ++) {
			for (j = 0; j < 8; j ++) {

				if ((c[i] & 0x80) == 0x00) {
					thenode = thenode->left;
				} else {
					thenode = thenode->right;
				}

				if (thenode->c > -1) {
					o[cur_write_size] = thenode->c;
					cur_write_size ++;
					thenode = treeroot;

					if (cur_write_size == BLOCKSIZE) {
						safe_write(fdout, o, BLOCKSIZE);
						cur_write_size = 0;
					}
				}
				c[i] = c[i] << 1;
			}
		}
	}

	safe_write(fdout, o, cur_write_size);

}

