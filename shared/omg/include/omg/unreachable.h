#pragma once

#include "assert.h"

/* Raise an undefined behavior, equivalent to C++23 std::unreachable.
 * Use it instead of assert(false) to mark places that should not be reached.
 * This could be an OMG::unreachable() method, but we use a macro to shorten
 * backtrace during fuzzer debugging. */
#define OMG_UNREACHABLE      \
  {                          \
    assert(false);           \
    __builtin_unreachable(); \
  }
