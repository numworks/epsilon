#ifndef LIBA_STRING_H
#define LIBA_STRING_H

#include <stddef.h>

#include "private/macros.h"

LIBA_BEGIN_DECLS

int memcmp(const void* s1, const void* s2, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dst, const void* src, size_t n);
void* memset(void* b, int c, size_t len);

char* strchr(const char* s, int c);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
size_t strlcat(char* dst, const char* src, size_t dstSize);
size_t strlcpy(char* dst, const char* src, size_t dstSize);
size_t strlen(const char* s);

LIBA_END_DECLS

#endif
