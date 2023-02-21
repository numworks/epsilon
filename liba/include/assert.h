#ifndef LIBA_ASSERT_H
#define LIBA_ASSERT_H

#include "private/macros.h"

#if ASSERTIONS
#define assert(e) ((void)((e) ? ((void)0) : __assert(#e, __FILE__, __LINE__)))
#else
#define assert(e) ((void)0)
#endif

LIBA_BEGIN_DECLS

void __assert(const char* expression, const char* file, int line);

LIBA_END_DECLS

#endif
