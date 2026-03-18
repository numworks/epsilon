#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

/* fsub and dsub are nearly identical to fadd and dadd which looses
 * some precious binary size. We simply call fadd from fsub after
 * changing the sign bit. */

[[gnu::noinline, gnu::externally_visible]] float __aeabi_fsub(float a,
                                                              float b) {
  *((uint32_t*)&b) ^= 0x80000000;  // flip sign bit
  return a + b;                    // i.e. call __aeabi_fadd
}

[[gnu::noinline, gnu::externally_visible]] double __aeabi_dsub(double a,
                                                               double b) {
  ((uint32_t*)&b)[1] ^= 0x80000000;  // flip sign bit
  return a + b;                      // i.e. call __aeabi_dadd
}

#pragma GCC diagnostic pop
