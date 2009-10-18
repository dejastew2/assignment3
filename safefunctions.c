#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BLOCKSIZE 10

void *safe_malloc(size_t size) {
	void* ret;
	ret = malloc(size);
	if (ret != NULL) {
		return ret;
	} else {
		perror("safe_malloc");
		exit(1);
	}
}

void *safe_realloc(void *p, size_t size) {
	void* ret;
	ret = realloc(p, size);
	if (ret != NULL) {
		return ret;
	} else {
		perror("safe_realloc");
		exit(1);
	}
}

char *get_next_word(FILE *pfile) {
	char *myword;
	char c;
	int pos = 0;
	int totalsize = BLOCKSIZE;
	int startword = 0;

	/* If the passed file exists */
	if (pfile) {

		myword = (char *)safe_malloc(BLOCKSIZE);

		while((c = getc(pfile)) != EOF) {

			/* If we've hit a char, flag it and add it */
			if (isalpha(c)) {
				if (!startword) {
					startword = 1;
				}

				/* If current string is too small, realloc */
				if (pos >= totalsize) {
					totalsize += BLOCKSIZE;
					myword = (char *)safe_realloc(myword, totalsize);
				}
				/* Add lowercase char to word */
				myword[pos] = tolower(c);
				pos ++;

			/* If we have a non-alphabetic char */
			} else {
				/* If we just hit the end of a word, break loop */
				if (startword) {
					/* Checks to see if string is just long enough */
					if (pos >= totalsize) {
						myword = (char *)safe_realloc(myword, totalsize);
					}
					myword[pos] = '\0';
					ungetc(c, pfile);
					break;
				}
			}

		}

	/* If the passed file is invalid */
	} else {
		myword = NULL;
	}

	/* If word is empty (ie at EOF), return NULL */
	if(strlen(myword) == 0) {
		free(myword);
		myword = NULL;
	}

	return myword;
}
