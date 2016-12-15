#ifndef LIBA_MATH_H
#define LIBA_MATH_H

#include "private/macros.h"

LIBA_BEGIN_DECLS

#define NAN (0.0f/0.0f)
#define M_E 2.71828182845904523536028747135266250
#define M_PI 3.14159265358979323846264338327950288

/* The C99 standard requires isinf and isnan to be defined as macros that can
 * handle arbitrary precision float numbers. The names of the functions called
 * by those macros (depending on the argument size) are not standardized though.
 * We're chosing isinff/isnanf for single-precision functions (which is the only
 * case we're actually handling). */

int isinff(float x);
#define isinf(x) isinff(x)
int isnanf(float x);
#define isnan(x) isnanf(x)

float ceilf(float x);
float copysignf(float x, float y);
float cosf(float x);
float expf(float x);
float fabsf(float x);
float floorf(float x);
float lgammaf(float x);
float lgammaf_r(float x, int *signgamp);
float log10f(float x);
float logf(float x);
float powf(float x, float y);
float roundf(float x);
float scalbnf(float x, int n);
float sinf(float x);
float sqrtf(float x);
float tanf(float x);

LIBA_END_DECLS

#endif
