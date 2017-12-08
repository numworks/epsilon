// This file tests that each math fuction links.
#include <math.h>

int test_fpclassifyf(float x) { return fpclassify(x); }
int test_signbitf(float x) { return signbit(x); }
int test_finitef(float x) { return finite(x); }
int test_isfinitef(float x) { return isfinite(x); }
int test_isnormalf(float x) { return isnormal(x); }
int test_isnanf(float x) { return isnan(x); }
int test_isinff(float x) { return isinf(x); }

float test_acosf(float x) { return acosf(x); }
float test_acoshf(float x) { return acoshf(x); }
float test_asinf(float x) { return asinf(x); }
float test_asinhf(float x) { return asinhf(x); }
float test_atanf(float x) { return atanf(x); }
float test_atan2f(float y, float x) { return atan2f(y, x); }
float test_atanhf(float x) { return atanhf(x); }
float test_ceilf(float x) { return ceilf(x); }
float test_copysignf(float x, float y) { return copysignf(x, y); }
float test_cosf(float x) { return cosf(x); }
float test_coshf(float x) { return coshf(x); }
float test_expf(float x) { return expf(x); }
float test_expm1f(float x) { return expm1f(x); }
float test_fabsf(float x) { return fabsf(x); }
float test_floorf(float x) { return floorf(x); }
float test_fmodf(float x, float y) { return fmodf(x, y); }
float test_frexpf(float x, int *exp) { return frexpf(x, exp); }
float test_ldexpf(float x, int exp) { return ldexpf(x, exp); }
float test_lgammaf(float x) { return lgammaf(x); }
float test_lgammaf_r(float x, int *signgamp) { return lgammaf_r(x, signgamp); }
float test_log1pf(float x) { return log1pf(x); }
float test_log10f(float x) { return log10f(x); }
float test_logf(float x) { return logf(x); }
float test_modff(float x, float *iptr) { return modff(x, iptr); }
float test_nanf(const char *s) { return nanf(s); }
float test_nearbyintf(float x) { return nearbyintf(x); }
float test_powf(float x, float y) { return powf(x, y); }
float test_roundf(float x) { return roundf(x); }
float test_scalbnf(float x, int exp) { return scalbnf(x, exp); }
float test_sinf(float x) { return sinf(x); }
float test_sinhf(float x) { return sinhf(x); }
float test_sqrtf(float x) { return sqrtf(x); }
float test_tanf(float x) { return tanf(x); }
float test_tanhf(float x) { return tanhf(x); }
float test_truncf(float x) { return truncf(x); }

int test_fpclassify(double x) { return fpclassify(x); }
int test_signbit(double x) { return signbit(x); }
int test_finite(double x) { return finite(x); }
int test_isfinite(double x) { return isfinite(x); }
int test_isnormal(double x) { return isnormal(x); }
int test_isnan(double x) { return isnan(x); }
int test_isinf(double x) { return isinf(x); }

double test_acos(double x) { return acos(x); }
double test_acosh(double x) { return acosh(x); }
double test_asin(double x) { return asin(x); }
double test_asinh(double x) { return asinh(x); }
double test_atan(double x) { return atan(x); }
double test_atan2(double y, double x) { return atan2(y, x); }
double test_atanh(double x) { return atanh(x); }
double test_ceil(double x) { return ceil(x); }
double test_copysign(double x, double y) { return copysign(x, y); }
double test_cos(double x) { return cos(x); }
double test_cosh(double x) { return cosh(x); }
double test_erf(double x) { return erf(x); }
double test_erfc(double x) { return erfc(x); }
double test_exp(double x) { return exp(x); }
double test_expm1(double x) { return expm1(x); }
double test_fabs(double x) { return fabs(x); }
double test_floor(double x) { return floor(x); }
double test_fmod(double x, double y) { return fmod(x, y); }
double test_frexp(double x, int *exp) { return frexp(x, exp); }
double test_ldexp(double x, int exp) { return ldexp(x, exp); }
double test_lgamma(double x) { return lgamma(x); }
double test_lgamma_r(double x, int *signgamp) { return lgamma_r(x, signgamp); }
double test_log(double x) { return log(x); }
double test_log1p(double x) { return log1p(x); }
double test_log10(double x) { return log10(x); }
double test_log2(double x) { return log2(x); }
double test_logb(double x) { return logb(x); }
double test_modf(double x, double *iptr) { return modf(x, iptr); }
double test_nan(const char *s) { return nan(s); }
double test_nearbyint(double x) { return nearbyint(x); }
double test_pow(double x, double y) { return pow(x, y); }
double test_rint(double x) { return rint(x); }
double test_round(double x) { return round(x); }
double test_scalb(double x, double exp) { return scalb(x, exp); }
double test_scalbn(double x, int exp) { return scalbn(x, exp); }
double test_sin(double x) { return sin(x); }
double test_sinh(double x) { return sinh(x); }
double test_sqrt(double x) { return sqrt(x); }
double test_tan(double x) { return tan(x); }
double test_tanh(double x) { return tanh(x); }
double test_tgamma(double x) { return tgamma(x); }
double test_trunc(double x) { return trunc(x); }
