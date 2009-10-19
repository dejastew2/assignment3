#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct h_node node;
struct h_node {
	char c;
	int count;
	node *left;
	node *right;
	node *parent;
};

node *build_h_tree(int fd);
node *add_to_tree(node *root, char myc, int mycount);

int main(int argc, char *argv[]) {
	int infile, outfile;

	/* Checks to see if output file arg passed */
	if (argc > 2) {
		outfile = open(argv[2], O_WRONLY | O_TRUNC);
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

	build_h_tree(infile);
	lseek(infile, 0, SEEK_SET);

	/* write(outfile, &c, 1); */

	/* Closes any open files */
	close(infile);
	if (outfile != STDOUT_FILENO) {
		close(outfile);
	}

	return 0;
}

node *build_h_tree(int fd) {
	int counts[256];
	int totalchars;
	char c;
	int i, cursmallest;
	node *root = NULL;

	/* Loads the counter array with zeroes */
	for (i = 0; i < 256; i ++) {
		counts[i] = 0;
	}
	totalchars = 0;

	/* Builds counters with char counts */
	while (read(fd, &c, 1) > 0) {
		counts[(int)c] ++;
		totalchars ++;
	}

	printf("Total chars: %d (0x%x)\n", totalchars, totalchars);

	while (totalchars > 0) {
		cursmallest = 0;
		for (i = 0; i < 256; i ++) {
			if (counts[i] > 0) {
				if (counts[cursmallest] > 0) {
					if (counts[i] < counts[cursmallest]) {
						cursmallest = i;
					}
				} else {
					cursmallest = i;
				}
			}
		}
		printf("%c %d\n", cursmallest, counts[cursmallest]);
		totalchars -= counts[cursmallest];
		counts[cursmallest] = 0;
	}

	return root;
}

node *add_to_tree(node *root, char myc, int mycount) {
	return root;
}
