#include "safefunctions.h"

#include <ctype.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>

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

int safe_open(const char *pathname, int flags, mode_t mode) {
	int ret = open(pathname, flags, mode);
	if (ret > -1)
		return ret;
	else {
		perror("safe_open");
		exit(1);
	}
}

ssize_t safe_read(int fd, void *buf, size_t count) {
	ssize_t ret = read(fd, buf, count);
	if (ret > -1) 
		return ret;
	else {
		perror("safe_read");
		exit(1);
	}
}

ssize_t safe_write(int fd, void *buf, size_t count) {
	ssize_t ret = write(fd, buf, count);
	if (ret == count) 
		return ret;
	else if (ret == -1) {
		perror("safe_write");
		exit(1);
	} else {
		perror("safe_write");
		exit(1);
	}
}

int safe_close(int fd) {
	int ret = close(fd);
	if (ret == 0)
		return ret;
	else {
		perror("safe_close");
		exit(1);
	}
}
