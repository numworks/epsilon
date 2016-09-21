#ifndef LIBA_STDDEF_H
#define LIBA_STDDEF_H

#define NULL 0

/* C99 says that size_t is an unsigned integer type of at least 16 bit.
 * Moreover, size_t is the return type of the "sizeof" operator, which is
 * defined by the compiler. That's why both GCC and clang define a __SIZE_TYPE__
 * macro to let the libc header know about the type used by the compiler. */
typedef __SIZE_TYPE__ size_t;

#endif
