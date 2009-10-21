#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

