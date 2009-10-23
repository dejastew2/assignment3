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
#define MAXPARENTDEPTH 256

/* Creates the header and builds search tree */
node *create_header(int fdin, int fdout);

/* Encodes fdin to fdout using search tree */
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

	/* Builds search tree, resets file pointer, then encodes file */
	treeroot = create_header(infile, outfile);
	lseek(infile, 0, SEEK_SET);
	encode(infile, outfile, treeroot);

	/* Closes any open files */
	safe_close(infile);
	if (outfile != STDOUT_FILENO)
		safe_close(outfile);

	/* Frees the tree */
	free_tree(treeroot);

	return 0;
}

node *create_header(int fdin, int fdout) {
	int counts[256];
	int totalchars, distinctchars;
	int read_size;
	int i;
	char c[BLOCKSIZE];
	node *root;

	/* Loads the counter array with zeroes */
	for (i = 0; i < 256; i ++) {
		counts[i] = 0;
	}
	totalchars = 0;

	/* Builds counters with char counts block by block */
	while ((read_size = safe_read(fdin, c, BLOCKSIZE)) > 0) {
		for (i = 0; i < read_size; i ++) {
			counts[(int)(c[i])] ++;
			totalchars ++;
		}
	}

	/* Counts distinct chars */
	distinctchars = 0;
	for (i = 0; i < 256; i ++) {
		if (counts[i] > 0)
			distinctchars ++;
	}

	/* Write header to file */
	safe_write(fdout, &distinctchars, sizeof(int));
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
	char c[BLOCKSIZE];
	char o[BLOCKSIZE];
	int read_size;
	int cur_write_size = 0;
	int parentinfo[MAXPARENTDEPTH];	/* HOW LONG CAN PARENT CODE BE??? */
	int i, j;		/* DO NEWLINES COUNT AS A CHAR FOR GRADING? */

	while ((read_size = safe_read(fdin, c, BLOCKSIZE)) > 0) {

		for (j = 0; j < read_size; j ++) {
			node *thenode = bfs(treeroot, c[j]);

			/* Reset all parent data */
			for (i = 0; i < MAXPARENTDEPTH; i ++) {
				parentinfo[i] = -1;
			}
			i = 0;
			/* Move back up to the root, setting parent code */
			while (thenode->parent != NULL) {
				if (thenode->parent->left == thenode)
					parentinfo[i] = 0;
				else
					parentinfo[i] = 1;

				thenode = thenode->parent;
				i ++;
			}

			/* Decode parent info  */
			for (i = (MAXPARENTDEPTH - 1); i >= 0; i --) {
				/* If parent code exists */
				if (parentinfo[i] > -1) {
					/* If zero, push in a zero from right */
					if (parentinfo[i] == 0)
						code = (code << 1) & 0xfe;
					/* If one, push in a one from right */
					else
						code = (code << 1) | 0x01;

					location ++;
					/* If at the end of byte, add to o */
					if (location > 7) {
						o[cur_write_size] = code;
						cur_write_size ++;
						location = 0;
						code = 0;

					/* If at end of block, write to file */
					if (cur_write_size == BLOCKSIZE) {
						safe_write(fdout, o, BLOCKSIZE);
						cur_write_size = 0;
					}
					}
				}
			}
		}

	}

	/* If last byte nonempty, fill with zeros and write to file */
	if (location != 0) {
		for (i = location; i < 8; i ++)
			code = (code << 1) & 0xfe;

		o[cur_write_size] = code;
		cur_write_size ++;
	}

	/* If we have a sub-block left over, write it to fdout */
	safe_write(fdout, o, cur_write_size);
}

