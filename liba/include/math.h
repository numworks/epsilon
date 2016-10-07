#ifndef LIBA_MATH_H
#define LIBA_MATH_H

#include "private/macros.h"

LIBA_BEGIN_DECLS

float logf(float x);
float log10f(float x);
float powf(float x, float y);
float fabsf(float x);
float sqrtf(float x);
float scalbnf(float x, int n);
float copysignf(float x, float y);

LIBA_END_DECLS

#endif
