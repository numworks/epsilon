#pragma once

#include "assert.h"

// Define custom assertions with special properties.

// ASSERTIONS and NDEBUG should be opposite
#if NDEBUG
static_assert(!ASSERTIONS);
#else
static_assert(ASSERTIONS);
#endif

// Indicates a severe risk if it fails in release
#define DANGER_ASSERT(c) assert(c)

// Always ignored in DEBUG=0
#define DEBUG_ASSERT(c) assert(!DEBUG || (c))

// Always ignored in DEBUG=1
#define NON_DEBUG_ASSERT(c) assert(DEBUG || (c))

// Always evaluates the condition
#if NDEBUG
#define EVALUATE_AND_ASSERT(c) ((void)(c))
#else
#define EVALUATE_AND_ASSERT(c) assert(c)
#endif

/* Ideas to implement in the future:
 * - NON_RECURSIVE_ASSERT : Ignored if called during an assertion
 */
