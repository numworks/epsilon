#ifndef LIBA_STRING_H
#define LIBA_STRING_H

#include "private/macros.h"
#include <stddef.h>

LIBA_BEGIN_DECLS

void * memcpy(void * dst, const void * src, size_t n);
void * memset(void * b, int c, size_t len);
size_t strlen(const char * s);
int strcmp(const char *s1, const char *s2);

LIBA_END_DECLS

#endif
