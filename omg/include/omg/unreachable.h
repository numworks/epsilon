#ifndef OMG_UNREACHABLE_H
#define OMG_UNREACHABLE_H

namespace OMG {

/* Raise an undefined behavior, equivalent to C++23 std::unreachable.
 * Use it instead of assert(false) to mark places that should not be reached.
 */
[[noreturn]] inline void unreachable() { __builtin_unreachable(); }

}  // namespace OMG

#endif
