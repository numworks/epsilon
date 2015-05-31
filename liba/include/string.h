#ifndef LIBA_STRING_H
#define LIBA_STRING_H

#include "private/macros.h"
#include "private/types.h"

#define NULL 0

LIBA_BEGIN_DECLS

void * memcpy(void * dst, const void * src, size_t n);
void * memset(void * b, int c, size_t len);
size_t strlen(const char * s);

LIBA_END_DECLS

#endif
