/* See the "Run-time ABI for the ARM Architecture", Section 4.1.2 */

#include "float.h"

// Conversions

aeabi_float_t __aeabi_l2f(int64_t i) {
  return f(i64_to_f32(i));
}

aeabi_double_t __aeabi_l2d(int64_t i) {
  return d(i64_to_f64(i));
}

aeabi_double_t __aeabi_ul2d(uint64_t i) {
  return d(ui64_to_f64(i));
}

int64_t __aeabi_f2lz(aeabi_float_t x) {
  return f32_to_i64_r_minMag(f32(x), 0);
}

int64_t __aeabi_d2lz(aeabi_double_t x) {
  return f64_to_i64_r_minMag(f64(x), 0);
}
