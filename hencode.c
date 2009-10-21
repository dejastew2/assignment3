#include "safefunctions.h"
#include "treesearch.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void encode(int fdin, int fdout, struct h_node *treeroot);

int main(int argc, char *argv[]) {
	int infile, outfile;
	struct h_node *treeroot;

	/* Checks to see if output file arg passed */
	if (argc > 2) {
		outfile = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (outfile == -1) {
			printf("Invalid output file %s\n", argv[2]);
			exit(1);
		}
	/* If no output file specified, use stdout */
	} else {
		outfile = STDOUT_FILENO;
	}

	/* Checks to see if input file arg passed */
	if (argc > 1) {
		infile = open(argv[1], O_RDONLY);
		if (infile == -1) {
			printf("Invalid input file %s\n", argv[1]);
			exit(1);
		}
	} else {
		printf("No file passed!\n");
		exit(1);
	}

	treeroot = build_h_tree(infile, outfile);
	lseek(infile, 0, SEEK_SET);
	encode(infile, outfile, treeroot);

	/* Closes any open files */
	close(infile);
	if (outfile != STDOUT_FILENO) {
		close(outfile);
	}

	return 0;
}

void encode(int fdin, int fdout, struct h_node *treeroot) {
	char code = 0;
	int location = 0;
	char c;
	int parentinfo[8];
	int i;

	while (read(fdin, &c, sizeof(char)) > 0) {

		struct h_node *thenode = bfs(treeroot, c);

		for (i = 0; i < 8; i ++) {
			parentinfo[i] = -1;
		}
		i = 0;
		while (thenode->parent != NULL) {
			if (thenode->parent->left == thenode) {
				parentinfo[i] = 0;
			} else {
				parentinfo[i] = 1;
			}
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
					write(fdout, &code, sizeof(char));
					location = 0;
					code = 0;
				}
			}
		}

	}

	if (location != 0) {
		for (i = location; i < 8; i ++)
			code = (code << 1) & 0xfe;

		write(fdout, &code, sizeof(char));
	}
}

