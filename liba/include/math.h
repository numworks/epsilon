#ifndef LIBA_MATH_H
#define LIBA_MATH_H

#include "private/macros.h"

LIBA_BEGIN_DECLS

float copysignf(float x, float y);
float cosf(float x);
float fabsf(float x);
float floorf(float x);
float log10f(float x);
float logf(float x);
float powf(float x, float y);
float scalbnf(float x, int n);
float sinf(float x);
float sqrtf(float x);
float tanf(float x);

LIBA_END_DECLS

#endif
