#pragma once

#include "assert.h"

/* Define special assertions with special properties. If applicable, prefer
 * using them, rather than a simple assert :
 * - assert : Standard assertion, can be deactivated with ASSERTIONS=0.
 * - DEBUG_ASSERT : Deactivated if DEBUG=0, even if ASSERTIONS=1.
 *                  Use them for assertions that are only useful during
 *                  debugging, and that can be skipped by CI or fuzzer.
 * - COSTLY_ASSERT : Deactivated if OPTIMIZE_ASSERTIONS=1, even if ASSERTIONS=1.
 *                   This should only be used for assertions that are costly to
 *                   evaluate, whether they are "important" or not.
 * - EVALUATE_AND_ASSERT : Always evaluates the condition.
 *                         Use them with non const methods that needs to be
 *                         called anyway, to prevent having to void temporary
 *                         variables for compilation warnings.
 *
 * Ideas to implement in the future:
 * - NON_RECURSIVE_ASSERT : Ignored if called during an assertion
 */

// ASSERTIONS and NDEBUG should be opposite
#if NDEBUG
static_assert(!ASSERTIONS);
#else
static_assert(ASSERTIONS);
#endif

#define DEBUG_ASSERT(c) assert(!DEBUG || (c))

#if OPTIMIZE_ASSERTIONS
#define COSTLY_ASSERT(c) ((void)0)
#else
#define COSTLY_ASSERT(c) assert(c)
#endif

#if NDEBUG
#define EVALUATE_AND_ASSERT(c) ((void)(c))
#else
#define EVALUATE_AND_ASSERT(c) assert(c)
#endif
