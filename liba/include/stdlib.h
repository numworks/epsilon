#ifndef LIBA_MALLOC_H
#define LIBA_MALLOC_H

#include "private/macros.h"
#include "private/types.h"

LIBA_BEGIN_DECLS

void free(void *ptr);
void * malloc(size_t size);
void * realloc(void *ptr, size_t size);

void abort(void);

LIBA_END_DECLS

#endif
