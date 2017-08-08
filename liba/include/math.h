#ifndef LIBA_MATH_H
#define LIBA_MATH_H

#include "private/macros.h"
#include <float.h>

LIBA_BEGIN_DECLS

#define NAN (0.0f/0.0f)
#define INFINITY __builtin_inff()
#define M_E 2.71828182845904523536028747135266250f
#define M_PI 3.14159265358979323846264338327950288f

/* The C99 standard requires isinf and isnan to be defined as macros that can
 * handle arbitrary precision float numbers. The names of the functions called
 * by those macros (depending on the argument size) are not standardized though.
 * We're chosing isinff/isnanf for single-precision functions, and isinfd/isnand
 * for double-precision functions. */

int isinff(float x);
int isinfd(double d);
#define isinf(x) (sizeof(x) == sizeof(float) ? isinff(x) : isinfd(x))
int isnanf(float x);
int isnand(double x);
#define isnan(x) (sizeof(x) == sizeof(float) ? isnanf(x) : isnand(x))

float acosf(float x);
float acoshf(float x);
float asinf(float x);
float asinhf(float x);
float atanf(float x);
float atanhf(float x);
float ceilf(float x);
float copysignf(float x, float y);
float cosf(float x);
float coshf(float x);
float expf(float x);
float expm1f(float x);
float fabsf(float x);
float floorf(float x);
float lgammaf(float x);
float lgammaf_r(float x, int *signgamp);
float log1pf(float x);
float log10f(float x);
float logf(float x);
float powf(float x, float y);
float roundf(float x);
float scalbnf(float x, int n);
float sinf(float x);
float sinhf(float x);
float sqrtf(float x);
float tanf(float x);
float tanhf(float x);

double acos(double x);
double acosh(double x);
double asin(double x);
double asinh(double x);
double atan(double x);
double atanh(double x);
double ceil(double x);
double copysign(double x, double y);
double cos(double x);
double cosh(double x);
double exp(double x);
double expm1(double x);
double fabs(double x);
double floor(double x);
double lgamma(double x);
double lgamma_r(double x, int *signgamp);
double log1p(double x);
double log10(double x);
double log(double x);
double pow(double x, double y);
double round(double x);
double scalbn(double x, int n);
double sin(double x);
double sinh(double x);
double sqrt(double x);
double tan(double x);
double tanh(double x);

LIBA_END_DECLS

#endif
