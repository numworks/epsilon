#ifndef OMG_UNREACHABLE_H
#define OMG_UNREACHABLE_H

#include "assert.h"

namespace OMG {

/* Raise an undefined behavior, equivalent to C++23 std::unreachable.
 * Use it instead of assert(false) to mark places that should not be reached.
 */
[[noreturn]] inline void unreachable() {
  assert(false);
  __builtin_unreachable();
}

}  // namespace OMG

#endif
