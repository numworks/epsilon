/* See the "Run-time ABI for the ARM Architecture", Section 4.1.2 */

#include <assert.h>
#include "../external/softfloat/port/softfloat.h"

/* To avoid any issue due to procedure call conventions, we convert softfloat
 * double 'aeabi_double_t' (respectively 'aeabi_float_t') to float64_t (to
 * respectively float32_t) before or/and after calling softfloat functions. */

typedef unsigned aeabi_float_t;
typedef unsigned long long aeabi_double_t;

static inline float32_t f32(aeabi_float_t f) {
  union {
    aeabi_float_t in;
    float32_t out;
  } res = {.in = f};
  assert(sizeof(aeabi_float_t) == sizeof(float32_t));
  assert(sizeof(res) == sizeof(float32_t));
  return res.out;
}

static inline aeabi_float_t f(float32_t f) {
  union {
    float32_t in;
    aeabi_float_t out;
  } res = {.in = f};
  assert(sizeof(aeabi_float_t) == sizeof(float32_t));
  assert(sizeof(res) == sizeof(aeabi_float_t));
  return res.out;
}

static inline float64_t f64(aeabi_double_t d) {
  union {
    aeabi_double_t in;
    float64_t out;
  } res = {.in = d};
  assert(sizeof(aeabi_double_t) == sizeof(float64_t));
  assert(sizeof(res) == sizeof(float64_t));
  return res.out;
}


static inline aeabi_double_t d(float64_t d) {
  union {
    float64_t in;
    aeabi_double_t out;
  } res = {.in = d};
  assert(sizeof(aeabi_double_t) == sizeof(float64_t));
  assert(sizeof(res) == sizeof(aeabi_double_t));
  return res.out;
}

// Conversions

int __aeabi_d2iz(aeabi_double_t x) {
  return f64_to_i32_r_minMag(f64(x), 0);
}

aeabi_double_t __aeabi_i2d(int i) {
  return d(i32_to_f64(i));
}

aeabi_float_t __aeabi_d2f(aeabi_double_t d) {
  return f(f64_to_f32(f64(d)));
}

aeabi_double_t __aeabi_f2d(aeabi_float_t f) {
  return d(f32_to_f64(f32(f)));
}

// Comparisons

int __aeabi_dcmpeq(aeabi_double_t a, aeabi_double_t b) {
  return f64_eq(f64(a), f64(b));
}

int __aeabi_dcmpge(aeabi_double_t a, aeabi_double_t b) {
  return f64_le(f64(b), f64(a));
}

int __aeabi_dcmpgt(aeabi_double_t a, aeabi_double_t b) {
  return f64_lt(f64(b), f64(a));
}

int __aeabi_dcmple(aeabi_double_t a, aeabi_double_t b) {
  return f64_le(f64(a), f64(b));
}

int __aeabi_dcmplt(aeabi_double_t a, aeabi_double_t b) {
  return f64_lt(f64(a), f64(b));
}

// Arithmetics

aeabi_double_t __aeabi_dadd(aeabi_double_t a, aeabi_double_t b) {
  return d(f64_add(f64(a), f64(b)));
}

aeabi_double_t __aeabi_dsub(aeabi_double_t a, aeabi_double_t b) {
  return d(f64_sub(f64(a), f64(b)));
}

aeabi_double_t __aeabi_dmul(aeabi_double_t a, aeabi_double_t b) {
  return d(f64_mul(f64(a), f64(b)));
}

aeabi_double_t __aeabi_ddiv(aeabi_double_t a, aeabi_double_t b) {
  return d(f64_div(f64(a), f64(b)));
}
