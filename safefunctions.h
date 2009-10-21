#ifndef SAFEFUNCTIONS_H_INCLUDED
#define SAFEFUNCTIONS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

/* This adds error checking to malloc */
void *safe_malloc(size_t size);

/* This adds error checking to realloc */
void *safe_realloc(void *p, size_t size);

/* This adds error checking to system call open */
int safe_open(const char *pathname, int flags, mode_t mode);

/* This adds error checking to system call read */
ssize_t safe_read(int fd, void *buf, size_t count);

/* This adds error checking to system call write */
ssize_t safe_write(int fd, void *buf, size_t count);

/* This adds error checking to system call close */
int safe_close(int fd);

#endif
