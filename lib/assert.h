#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define assert(e) ((void)0)
#else
#define assert(e)  ((void) ((e) ? ((void)0) : __assert(#e, __FILE__, __LINE__)))
#endif

void __assert(const char * expression, const char * file, int line);

void abort(void);

#endif
