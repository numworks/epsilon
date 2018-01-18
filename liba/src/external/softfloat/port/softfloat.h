#ifndef LIBA_SOFTFLOAT_H
#define LIBA_SOFTFLOAT_H

/* As instructed in the softfloat documentation, we do not expose the original
 * softfloat.h header. Instead, we create our own. */

#include <stdbool.h>
#include <stdint.h>

typedef struct { uint32_t v; } float32_t;
typedef struct { uint64_t v; } float64_t;

float32_t f64_to_f32(float64_t x);
float64_t f32_to_f64(float32_t x);
float64_t i32_to_f64(int32_t i);
float64_t ui32_to_f64(uint32_t i);
float64_t ui64_to_f64(uint64_t i);
int_fast32_t f64_to_i32_r_minMag(float64_t x, bool exact);
bool f64_eq(float64_t x, float64_t y);
bool f64_le(float64_t x, float64_t y);
bool f64_lt(float64_t x, float64_t y);
float64_t f64_add(float64_t x, float64_t y);
float64_t f64_sub(float64_t x, float64_t y);
float64_t f64_mul(float64_t x, float64_t y);
float64_t f64_div(float64_t x, float64_t y);

float32_t i64_to_f32(int64_t i);
float64_t i64_to_f64(int64_t i);
int_fast64_t f32_to_i64_r_minMag(float32_t x, bool exact);
int_fast64_t f64_to_i64_r_minMag(float64_t x, bool exact);


#endif
