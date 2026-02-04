#ifndef LIBA_MALLOC_H
#define LIBA_MALLOC_H

#include <stddef.h>

#include "private/macros.h"

LIBA_BEGIN_DECLS

void abort(void) __attribute__((noreturn));
void exit(int code) __attribute__((noreturn));

int abs(int n);

LIBA_END_DECLS

#endif
