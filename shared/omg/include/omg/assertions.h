#pragma once

#include "assert.h"

// Define custom assertions with special properties.

// Like assert, but it always evaluates to true
#define ASSERT(c) (assert(c), true)

// Like assert, but it evaluates to the condition
#define ASSERT_AND_RETURN(c) (c || (assert(false), false))

// Like ASSERT. It indicates a severe risk if it fails in release
#define DANGER_ASSERT(c) ASSERT(c)

// Like ASSERT, but it is always ignored in DEBUG=0
#define DEBUG_ASSERT(c) (!DEBUG || ASSERT(c))

// Like ASSERT, but it is always ignored in DEBUG=1
#define NON_DEBUG_ASSERT(c) (DEBUG || ASSERT(c))

/* ASSERT_AND_RETURN cannot be combined with other assertions:
 * - ASSERT_AND_RETURN(DANGER_ASSERT(c)) or DANGER_ASSERT(ASSERT_AND_RETURN(c))
 *
 * Some other OMG assertions can be combined. For example:
 * - DEBUG_ASSERT(DANGER_ASSERT(c)) same as DANGER_ASSERT(DEBUG_ASSERT(c))
 */

/* Ideas to implement in the future:
 * - NON_RECURSIVE_ASSERT : Like ASSERT, ignored if called during an assertion
 */
