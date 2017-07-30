#ifndef LIBA_STDDEF_H
#define LIBA_STDDEF_H

#define NULL 0

typedef int ssize_t;
typedef unsigned int size_t;

#define offsetof(type, field) __builtin_offsetof(type, field)

#endif
