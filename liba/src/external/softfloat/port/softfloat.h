#ifndef LIBA_SOFTFLOAT_H
#define LIBA_SOFTFLOAT_H 1

/* As instructed in the softfloat documentation, we do not expose the original
 * softfloat.h header. Instead, we create our own. This has the great upside of
 * letting us declare float64_t as a double instead of a struct. Of course this
 * is valid if, and only if, sizeof(double) == 8. */

#include <stdbool.h>
#include <stdint.h>

typedef double float64_t;

uint_fast32_t f64_to_ui32_r_minMag(float64_t x, bool i);
bool f64_eq(float64_t x, float64_t y);
bool f64_le(float64_t x, float64_t y);
bool f64_lt(float64_t x, float64_t y);
float64_t f64_add(float64_t x, float64_t y);
float64_t f64_sub(float64_t x, float64_t y);
float64_t f64_mul(float64_t x, float64_t y);
float64_t f64_div(float64_t x, float64_t y);

#endif
