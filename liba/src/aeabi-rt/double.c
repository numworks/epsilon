/* See the "Run-time ABI for the ARM Architecture", Section 4.1.2 */

#include "../external/softfloat/port/softfloat.h"

// Conversions

int __aeabi_d2iz(double d) {
  return f64_to_i32_r_minMag(d, 0);
}

double __aeabi_i2d(int i) {
  return i32_to_f64(i);
}

// Comparisons

int __aeabi_dcmpeq(double a, double b) {
  return f64_eq(a, b);
}

int __aeabi_dcmpge(double a, double b) {
  return f64_le(b, a);
}

int __aeabi_dcmpgt(double a, double b) {
  return f64_lt(b, a);
}

int __aeabi_dcmple(double a, double b) {
  return f64_le(a, b);
}

int __aeabi_dcmplt(double a, double b) {
  return f64_lt(a, b);
}

// Arithmetics

double __aeabi_dadd(double a, double b) {
  return f64_add(a, b);
}

double __aeabi_dsub(double a, double b) {
  return f64_sub(a, b);
}

double __aeabi_dmul(double a, double b) {
  return f64_mul(a, b);
}

double __aeabi_ddiv(double a, double b) {
  return f64_div(a, b);
}
