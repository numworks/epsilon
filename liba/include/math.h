#ifndef LIBA_MATH_H
#define LIBA_MATH_H

#include <float.h>

#include "private/macros.h"

LIBA_BEGIN_DECLS

typedef float float_t;
typedef double double_t;

#define M_E 2.71828182845904524
#define M_LOG2E 1.44269504088896341
#define M_LOG10E 0.43429448190325183
#define M_LN2 0.69314718055994531
#define M_LN10 2.30258509299404568
#define M_PI 3.14159265358979324
#define M_PI_2 1.57079632679489662
#define M_PI_4 0.78539816339744831
#define M_1_PI 0.31830988618379067
#define M_2_PI 0.63661977236758134
#define M_2_SQRTPI 1.12837916709551257
#define M_SQRT2 1.41421356237309505
#define M_SQRT1_2 0.70710678118654752

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

#define finite(x) __builtin_finite(x)
#define fpclassify(x) \
  __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)
#define isfinite(x) __builtin_isfinite(x)
#define isnormal(x) __builtin_isnormal(x)
#define isnan(x) __builtin_isnan(x)
#define isinf(x) __builtin_isinf(x)
#define signbit(x) __builtin_signbit(x)

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
float fmaxf(float x, float y);
float floorf(float x);
float fmodf(float x, float y);
float frexpf(float x, int *exp);
float hypotf(float x, float y);
float ldexpf(float x, int exp);
float lgammaf(float x);
float lgammaf_r(float x, int *signgamp);
float log1pf(float x);
float log10f(float x);
float logf(float x);
float modff(float x, float *iptr);
float nearbyintf(float x);
float nextafterf(float from, float to);
float powf(float x, float y);
float roundf(float x);
float scalbnf(float x, int exp);
float sinf(float x);
float sinhf(float x);
float sqrtf(float x);
float tanf(float x);
float tanhf(float x);
float truncf(float x);

double acos(double x);
double acosh(double x);
double asin(double x);
double asinh(double x);
double atan(double x);
double atan2(double y, double x);
double atanh(double x);
double ceil(double x);
double copysign(double x, double y);
double cos(double x);
double cosh(double x);
double erf(double x);
double erfc(double x);
double exp(double x);
double expm1(double x);
double fabs(double x);
double fmax(double x, double y);
double floor(double x);
double fmod(double x, double y);
double frexp(double x, int *exp);
double hypot(double x, double y);
double lgamma(double x);
double lgamma_r(double x, int *signgamp);
double log(double x);
double log1p(double x);
double log10(double x);
double log2(double x);
double logb(double x);
double modf(double x, double *iptr);
double nearbyint(double x);
double nextafter(double from, double to);
double pow(double x, double y);
double rint(double x);
double round(double x);
double scalb(double x, double exp);
double scalbn(double x, int exp);
double sin(double x);
double sinh(double x);
double sqrt(double x);
double tan(double x);
double tanh(double x);
double tgamma(double x);
double trunc(double x);

extern int signgam;

/* The C99 standard says that any libc function can be re-declared as a macro.
 * (See N1124 paragraph 7.1.4). This means that C files willing to actually
 * implement said functions should either re-define the prototype or #undef the
 * macro. */

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
#define frexpf(x, exp) __builtin_frexpf(x, exp)
#define ldexpf(x, exp) __builtin_ldexpf(x, exp)
#define lgammaf(x) __builtin_lgammaf(x)
#define lgammaf_r(x, signgamp) __builtin_lgammaf_r(x, signgamp)
#define log1pf(x) __builtin_log1pf(x)
#define log10f(x) __builtin_log10f(x)
#define logf(x) __builtin_logf(x)
#define modff(x, iptr) __builtin_modff(x, iptr)
#define nanf(tagp) __builtin_nanf(tagp)
#define nearbyintf(x) __builtin_nearbyintf(x)
#define nextafterf(from, to) __builtin_nextafterf(from, to)
#define powf(x, y) __builtin_powf(x, y)
#define roundf(x) __builtin_roundf(x)
#define scalbnf(x, exp) __builtin_scalbnf(x, exp)
#define sinf(x) __builtin_sinf(x)
#define sinhf(x) __builtin_sinhf(x)
#define sqrtf(x) __builtin_sqrtf(x)
#define tanf(x) __builtin_tanf(x)
#define tanhf(x) __builtin_tanhf(x)
#define truncf(x) __builtin_truncf(x)

#define acos(x) __builtin_acos(x)
#define acosh(x) __builtin_acosh(x)
#define asin(x) __builtin_asin(x)
#define asinh(x) __builtin_asinh(x)
#define atan(x) __builtin_atan(x)
#define atan2(y, x) __builtin_atan2(y, x)
#define atanh(x) __builtin_atanh(x)
#define ceil(x) __builtin_ceil(x)
#define copysign(x, y) __builtin_copysign(x, y)
#define cos(x) __builtin_cos(x)
#define cosh(x) __builtin_cosh(x)
#define erf(x) __builtin_erf(x)
#define erfc(x) __builtin_erfc(x)
#define exp(x) __builtin_exp(x)
#define expm1(x) __builtin_expm1(x)
#define fabs(x) __builtin_fabs(x)
#define floor(x) __builtin_floor(x)
#define fmod(x, y) __builtin_fmod(x, y)
#define frexp(x, exp) __builtin_frexp(x, exp)
#define ldexp(x, exp) __builtin_scalbn(x, exp)
#define lgamma(x) __builtin_lgamma(x)
#define lgamma_r(x, signgamp) __builtin_lgamma_r(x, signgamp)
#define log(x) __builtin_log(x)
#define log1p(x) __builtin_log1p(x)
#define log10(x) __builtin_log10(x)
#define log2(x) __builtin_log2(x)
#define logb(x) __builtin_logb(x)
#define modf(x, iptr) __builtin_modf(x, iptr)
#define nan(tagp) __builtin_nan(tagp)
#define nearbyint(x) __builtin_nearbyint(x)
#define nextafter(from, to) __builtin_nextafter(from, to)
#define pow(x, y) __builtin_pow(x, y)
#define rint(x) __builtin_rint(x)
#define round(x) __builtin_round(x)
#define scalb(x, exp) __builtin_scalb(x, exp)
#define scalbn(x, exp) __builtin_scalbn(x, exp)
#define sin(x) __builtin_sin(x)
#define sinh(x) __builtin_sinh(x)
#define sqrt(x) __builtin_sqrt(x)
#define tan(x) __builtin_tan(x)
#define tanh(x) __builtin_tanh(x)
#define tgamma(x) __builtin_tgamma(x)
#define trunc(x) __builtin_trunc(x)

LIBA_END_DECLS

#endif
