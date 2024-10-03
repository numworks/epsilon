#ifndef OMG_UNALIGNED_H
#define OMG_UNALIGNED_H

#include <stdint.h>

namespace OMG {

/* Helpers to access unaligned integer types
 *
 * The only portable way to make unaligned accesses is to use memcpy and to rely
 * on the compiler to remove it when it is possible. In particular on the SM32,
 * uint16 unaligned accesses are always possible and uint32 unaligned accesses
 * are possible if they are standalone. Unfortuately the compiler is not able to
 * optimize them in the userland since memcpy is an opaque trampoline access.
 *
 * The workaround used here with an aligned attribute comes from emscripten
 * (where all accesses must be correctly aligned). It is supported by both clang
 * and GCC however it is a bit dangerous since the attribute is not preserved by
 * `using` aliases.
 *
 * Prefer the inline functions to the typedefs where possible.
 */

typedef uint16_t __attribute__((aligned(1))) unaligned_uint16_t;

inline uint16_t unalignedShort(const void* address) {
  return *static_cast<const unaligned_uint16_t*>(address);
}

inline void writeUnalignedShort(uint16_t value, void* address) {
  *static_cast<unaligned_uint16_t*>(address) = value;
}

typedef uint32_t __attribute__((aligned(1))) unaligned_uint32_t;

inline uint32_t unalignedLong(const void* address) {
  return *static_cast<const unaligned_uint32_t*>(address);
}

inline void writeUnalignedLong(uint32_t value, void* address) {
  *static_cast<unaligned_uint32_t*>(address) = value;
}

}  // namespace OMG

#endif
