#ifndef LIB_SQLITEINT_H
#define LIB_SQLITEINT_H

/* This header files allows the standalone compilation of mem5.c from SQLite.
 * That file provides a tested implementation of malloc/free/realloc. */

#define sqlite3GlobalConfig HeapConfig
#include <liba/include/private/memconfig.h>

/* SQLite wants to use its own integer types. Let's define them based from the
 * stdint ones */

#include <stdint.h>
typedef uint64_t u64;
typedef uint8_t u8;
typedef uint32_t u32;
typedef int64_t sqlite3_int64;

/* Mem5 uses memcpy */
#include <string.h>

/* Define a bunch of macros used by SQLite */

#define SQLITE_ENABLE_MEMSYS5 1
#define SQLITE_WSD
#define GLOBAL(t,v) v
#define sqlite3_log(...) ((void)0)
#define ALWAYS(X) (X)
#define UNUSED_PARAMETER(x) ((void)0)
#define SQLITE_OK 0

/* Completely ignore asserts: one of them contains a modulo, which our platform
 * doesn't support in hardware. This therefore translates to a __aeabi_idivmod
 * call, which we do not provide. */
#define assert(x) ((void)0)

// Ignore SQLite testing code
#define testcase(x) ((void)0)


/* SQLite provides a mutex facility, and uses it to protect its memory
 * allocations. We don't need thread-safety for now so let's just neuter the
 * mutexes */
typedef void sqlite3_mutex;
#define sqlite3_mutex_enter(x) ((void)0)
#define sqlite3_mutex_leave(x) ((void)0)
#define sqlite3MutexAlloc(x) ((void *)0)

/* This one is dangerous, but needed: memsys5Malloc and co are defined as static
 * but we will want to use them all around (after renaming them to their
 * standard names). So let's just remove any static. Warning: this could break
 * static variables. Luckily, there are none. */
#define static

/* SQLite exports its memory methods in a struct. We don't need it since we've
 * renamed them to standard names anyway. So let's just make it build. */
typedef void ** sqlite3_mem_methods;

#endif
