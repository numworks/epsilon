#ifndef LIBA_STRING_H
#define LIBA_STRING_H

#include "private/macros.h"
#include <stddef.h>

LIBA_BEGIN_DECLS

int memcmp(const void * s1, const void * s2, size_t n);
void * memcpy(void * dst, const void * src, size_t n);
void * memmove(void * dst, const void * src, size_t n);
void * memset(void * b, int c, size_t len);

char * strchr(const char * s, int c);
int strcmp(const char * s1, const char * s2);
size_t strlcpy(char * dst, const char * src, size_t len);
size_t strlen(const char * s);
char * strtok_r(char * s, const char * delim, char ** last);

LIBA_END_DECLS

#endif
