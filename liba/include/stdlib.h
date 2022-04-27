#ifndef LIBA_MALLOC_H
#define LIBA_MALLOC_H

#include "private/macros.h"
#include <stddef.h>

LIBA_BEGIN_DECLS

void free(void *ptr);
void * malloc(size_t size);
void * realloc(void *ptr, size_t size);
void * calloc(size_t count, size_t size);
char * itoa(int value, char *str, int base);

void abort(void) __attribute__((noreturn));

int abs(int n);

LIBA_END_DECLS

#endif
