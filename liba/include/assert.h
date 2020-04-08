#ifndef LIBA_ASSERT_H
#define LIBA_ASSERT_H

#include "private/macros.h"

#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#define assert(e)  ((void) ((e) ? ((void)0) : __assert(#e, __FILE__, __LINE__)))
#endif

LIBA_BEGIN_DECLS

void __assert(const char * expression, const char * file, int line);

LIBA_END_DECLS

#endif
