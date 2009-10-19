#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int infile, outfile;
	char c;
	
	if (argc > 2) {
		outfile = open(argv[2], O_WRONLY | O_TRUNC);
		if (outfile == -1) {
			printf("Invalid output file %s\n", argv[2]);
			exit(1);
		}
	} else {
		outfile = STDOUT_FILENO;
	}
	
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

	read(infile, &c, 1);

	write(outfile, &c, 1);

	close(infile);
	if (outfile != STDOUT_FILENO) {
		close(outfile);
	}
	
	return 0;
}