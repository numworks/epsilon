#ifndef LIBA_AEABI_FLOAT_H
#define LIBA_AEABI_FLOAT_H

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

#endif
