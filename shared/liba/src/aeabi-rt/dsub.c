#include <stdint.h>

/* fsub and dsub are nearly identical to fadd and dadd which loses
 * some precious binary size. We simply call fadd from fsub after
 * changing the sign bit. */

__attribute__((noinline, externally_visible)) float __aeabi_fsub(float a,
                                                                 float b) {
  union {
    float f;
    uint32_t u;
  } f;
  f.f = b;
  f.u ^= (1ul << 31);  // flip sign bit
  return a + f.f;      // i.e. call __aeabi_fadd
}

__attribute__((noinline, externally_visible)) double __aeabi_dsub(double a,
                                                                  double b) {
  union {
    double f;
    uint64_t u;
  } f;
  f.f = b;
  f.u ^= (1ull << 63);  // flip sign bit
  return a + f.f;       // i.e. call __aeabi_dadd
}
