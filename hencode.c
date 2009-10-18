#include "safefunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int build_h_table(int myfile);

int main(int argc, char *argv[]) {

	int infile = 0, outfile = 1;
	int n;
	char *buf = NULL;
	int i;

	printf("Hello %d", infile);

	/* Loops through each argument */
	for (i = 1; i < 3; i ++) {

		char* file_name;	/* Pointer to argument */

		switch (i) {
			case 1:
				if (i < argc) {
					/* Points file_name to argument (after allocating mem) */
					file_name = (char*)safe_malloc(strlen(argv[i]) + 1);
					strcpy(file_name, argv[i]);
					infile = open(file_name, O_RDONLY, 0);
				} else {
					infile = 0;
				}
				break;
			case 2:
				if (i < argc) {
					/* Points file_name to argument (after allocating mem) */
					file_name = (char*)safe_malloc(strlen(argv[i]) + 1);
					strcpy(file_name, argv[i]);
					outfile = open(file_name, O_WRONLY, 0);
				} else {
					outfile = 1;
				}
				break;
		}
	}

	build_h_table(infile);

	while ((n = read(infile, buf, 1)) > 0) {
		i = write(outfile, buf, n);
	}

	close(infile);
	close(outfile);

	return 0;
}

int build_h_table(int myfile) {
	return 0;
}
