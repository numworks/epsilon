/* This file patches the assert macro to raise an exception instead */

/* include_next is a GCC extension directive to include the next header with
 * that name in the search path to access the system one. */
#include_next "assert.h"

#ifdef __cplusplus

/* assert.h generally have only macros and no header guards (to be able to have
 * some part in DEBUG) only so here is a mini-header guard for us */
#ifndef _ExceptionCheckpointRaise_DECL_
#define _ExceptionCheckpointRaise_DECL_
extern "C" {
void ExceptionCheckpointRaise() __attribute__((__noreturn__));
}
#endif

#undef assert
#define assert(e) \
  (__builtin_expect(!(e), 0) ? ExceptionCheckpointRaise() : (void)0)
#endif
