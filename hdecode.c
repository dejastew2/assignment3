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

/* Reads header and builds search tree */
node *read_header(int fdin, int fdout, int *totalchars);

/* Decodes fdin to fdout using search tree */
void decode(int fdin, int fdout, node *treeroot, int totalchars);

int main(int argc, char *argv[]) {
	int infile, outfile;
	node *treeroot;
	int totalchars = 0;

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
	else
		infile = STDIN_FILENO;

	/* Builds search tree and decodes file */
	treeroot = read_header(infile, outfile, &totalchars);
	decode(infile, outfile, treeroot, totalchars);

	/* Closes any open files */
	if (infile != STDIN_FILENO)
		safe_close(infile);
	if (outfile != STDOUT_FILENO)
		safe_close(outfile);

	/* Frees the tree */
	free_tree(treeroot);

	return 0;
}

node *read_header(int fdin, int fdout, int *totalchars) {
	int counts[256];
	int temptcs;
	int i, tempcount;
	char c;
	node *root;

	/* Loads the counter array with zeroes */
	for (i = 0; i < 256; i ++) {
		counts[i] = 0;
	}
	*totalchars = 0;

	/* Reads in the total number of characters */
	safe_read(fdin, &temptcs, sizeof(int));

	while (temptcs > 0) {
		/* Reads in the character and the count */
		safe_read(fdin, &c, sizeof(char));
		safe_read(fdin, &tempcount, sizeof(int));

		/* Inserts into counts array */
		counts[(int)c] = tempcount;

		*totalchars += tempcount;
		temptcs -= 1;
	}

	/* Builds tree */
	root = build_h_tree(counts, *totalchars);

	return root;
}

void decode(int fdin, int fdout, node *treeroot, int totalchars) {
	char c[BLOCKSIZE];
	char o[BLOCKSIZE];
	int read_size;
	int cur_write_size = 0;
	int i, j;
	int num_chars = 0;
	int blank_file_flag = 1;

	node *thenode = treeroot;

	/* Reads the file block by block */
	while ((read_size = safe_read(fdin, c, BLOCKSIZE)) > 0) {

		blank_file_flag = 0;
		
		/* Moves through each char read into char array */
		for (i = 0; i < read_size; i ++) {
			/* Moves through each bit in individual char */
			for (j = 0; j < 8; j ++) {
				/* Moves the search node based on the bit */
				if ((c[i] & 0x80) == 0x00)
					thenode = thenode->left;
				else
					thenode = thenode->right;

				/* If we have found a valid char in the tree */
				if (thenode->c > -1) {
					if (num_chars < totalchars) {
					/* Write to output char array */
					o[cur_write_size] = thenode->c;
					cur_write_size ++;
					num_chars ++;

					/* If we have a block of output ready */
					if (cur_write_size == BLOCKSIZE) {
						safe_write(fdout, o, BLOCKSIZE);
						cur_write_size = 0;
					}
					}
					thenode = treeroot;
				}

				/* Shift next bit into position in char */
				c[i] = c[i] << 1;
			}
		}
	}
	
	if (blank_file_flag == 1) {
		lseek(fdin, 0, SEEK_SET);
		safe_read(fdin, c, sizeof(int));
		safe_read(fdin, c, 2);
		for (i = 0; i < c[1]; i ++) {
			o[cur_write_size] = c[0];
			cur_write_size ++;
			
			/* If we have a block of output ready */
			if (cur_write_size == BLOCKSIZE) {
				safe_write(fdout, o, BLOCKSIZE);
				cur_write_size = 0;
			}			
		}
	}

	/* If we have a sub-block left over, write it to fdout */
	safe_write(fdout, o, cur_write_size);

}

