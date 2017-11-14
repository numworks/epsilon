#include_next <math.h>

/* s_roundf.c uses isinff and isnanf. Let's correct this */

#define isinff(x) __builtin_isinf(x)
#define isnanf(x) __builtin_isnan(x)

/* In accordance with the C99 standard, we've defined libm function as macros to
 * leverage compiler optimizations. When comes the time to actually implement
 * those functions, we don't want the macro to be active. OpenBSD doesn't use
 * macros so it doesn't bother #undef-ing libm functions. Let's do it here. */

#undef acosf
#undef acoshf
#undef asinf
#undef asinhf
#undef atanf
#undef atan2f
#undef atanhf
#undef ceilf
#undef copysignf
#undef cosf
#undef coshf
#undef expf
#undef expm1f
#undef fabsf
#undef floorf
#undef fmodf
#undef ldexpf
#undef lgammaf
#undef lgammaf_r
#undef log1pf
#undef log10f
#undef logf
#undef nearbyintf
#undef powf
#undef roundf
#undef scalbnf
#undef sinf
#undef sinhf
#undef sqrtf
#undef tanf
#undef tanhf
#undef truncf

#undef acos
#undef acosh
#undef asin
#undef asinh
#undef atan
#undef atan2
#undef atanh
#undef ceil
#undef copysign
#undef cos
#undef cosh
#undef erf
#undef erfc
#undef exp
#undef expm1
#undef fabs
#undef floor
#undef fmod
#undef lgamma
#undef lgamma_r
#undef log
#undef log1p
#undef log10
#undef log2
#undef logb
#undef pow
#undef rint
#undef round
#undef scalbn
#undef sin
#undef sinh
#undef sqrt
#undef tgamma
#undef tan
#undef tanh
#undef trunc
