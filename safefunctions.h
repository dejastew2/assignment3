#ifndef SAFEFUNCTIONS_H_INCLUDED
#define SAFEFUNCTIONS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

/* This adds error checking to malloc */
void *safe_malloc(size_t size);

/* This adds error checking to realloc */
void *safe_realloc(void *p, size_t size);

#endif
