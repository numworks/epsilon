#ifndef LIBA_MATH_H
#define LIBA_MATH_H

#include "private/macros.h"
#include <float.h>

LIBA_BEGIN_DECLS

typedef float float_t;
typedef double double_t;

#define M_E        2.71828182845904524
#define M_LOG2E    1.44269504088896341
#define M_LOG10E   0.43429448190325183
#define M_LN2      0.69314718055994531
#define M_LN10     2.30258509299404568
#define M_PI       3.14159265358979324
#define M_PI_2     1.57079632679489662
#define M_PI_4     0.78539816339744831
#define M_1_PI     0.31830988618379067
#define M_2_PI     0.63661977236758134
#define M_2_SQRTPI 1.12837916709551257
#define M_SQRT2    1.41421356237309505
#define M_SQRT1_2  0.70710678118654752

#define MAXFLOAT FLT_MAX
#define HUGE_VAL __builtin_huge_val()
#define HUGE_VALF __builtin_huge_valf()
#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")

#define FP_INFINITE 0x01
#define FP_NAN 0x02
#define FP_NORMAL 0x04
#define FP_SUBNORMAL 0x08
#define FP_ZERO 0x10

#define fpclassify(x) __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#define isfinite(x) __builtin_isfinite(x)
#define isnormal(x) __builtin_isnormal(x)
#define isnan(x) __builtin_isnan(x)
#define isinf(x) __builtin_isinf(x)

float acosf(float x);
float acoshf(float x);
float asinf(float x);
float asinhf(float x);
float atanf(float x);
float atan2f(float y, float x);
float atanhf(float x);
float ceilf(float x);
float copysignf(float x, float y);
float cosf(float x);
float coshf(float x);
float expf(float x);
float expm1f(float x);
float fabsf(float x);
float floorf(float x);
float fmodf(float x, float y);
float lgammaf(float x);
float lgammaf_r(float x, int *signgamp);
float log1pf(float x);
float log10f(float x);
float logf(float x);
float nanf(const char *s);
float nearbyintf(float x);
float powf(float x, float y);
float roundf(float x);
float scalbnf(float x, int n);
float sinf(float x);
float sinhf(float x);
float sqrtf(float x);
float tanf(float x);
float tanhf(float x);

#define acosf(x) __builtin_acosf(x)
#define acoshf(x) __builtin_acoshf(x)
#define asinf(x) __builtin_asinf(x)
#define asinhf(x) __builtin_asinhf(x)
#define atanf(x) __builtin_atanf(x)
#define atan2f(y, x) __builtin_atan2f(y, x)
#define atanhf(x) __builtin_atanhf(x)
#define ceilf(x) __builtin_ceilf(x)
#define copysignf(x, y) __builtin_copysignf(x, y)
#define cosf(x) __builtin_cosf(x)
#define coshf(x) __builtin_coshf(x)
#define expf(x) __builtin_expf(x)
#define expm1f(x) __builtin_expm1f(x)
#define fabsf(x) __builtin_fabsf(x)
#define floorf(x) __builtin_floorf(x)
#define fmodf(x, y) __builtin_fmodf(x, y)
#define lgammaf(x) __builtin_lgammaf(x)
#define lgammaf_r(x, signgamp) __builtin_lgammaf_r(x, signgamp)
#define log1pf(x) __builtin_log1pf(x)
#define log10f(x) __builtin_log10f(x)
#define logf(x) __builtin_logf(x)
#define nanf(s) __builtin_nanf(s)
#define nearbyintf(x) __builtin_nearbyintf(x)
#define powf(x, y) __builtin_powf(x, y)
#define roundf(x) __builtin_roundf(x)
#define scalbnf(x, n) __builtin_scalbnf(x, n)
#define sinf(x) __builtin_sinf(x)
#define sinhf(x) __builtin_sinhf(x)
#define sqrtf(x) __builtin_sqrtf(x)
#define tanf(x) __builtin_tanf(x)
#define tanhf(x) __builtin_tanhf(x)

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

#define acos(x) __builtin_acos(x)
#define acosh(x) __builtin_acosh(x)
#define asin(x) __builtin_asin(x)
#define asinh(x) __builtin_asinh(x)
#define atan(x) __builtin_atan(x)
#define atanh(x) __builtin_atanh(x)
#define ceil(x) __builtin_ceil(x)
#define copysign(x, y) __builtin_copysign(x, y)
#define cos(x) __builtin_cos(x)
#define cosh(x) __builtin_cosh(x)
#define exp(x) __builtin_exp(x)
#define expm1(x) __builtin_expm1(x)
#define fabs(x) __builtin_fabs(x)
#define floor(x) __builtin_floor(x)
#define lgamma(x) __builtin_lgamma(x)
#define lgamma_r(x, signgamp) __builtin_lgamma_r(x, signgamp)
#define log1p(x) __builtin_log1p(x)
#define log10(x) __builtin_log10(x)
#define log(x) __builtin_log(x)
#define pow(x, y) __builtin_pow(x, y)
#define round(x) __builtin_round(x)
#define scalbn(x, n) __builtin_scalbn(x, n)
#define sin(x) __builtin_sin(x)
#define sinh(x) __builtin_sinh(x)
#define sqrt(x) __builtin_sqrt(x)
#define tan(x) __builtin_tan(x)
#define tanh(x) __builtin_tanh(x)

extern int signgam;

LIBA_END_DECLS

#endif
