#ifndef LIBA_SOFTFLOAT_H
#define LIBA_SOFTFLOAT_H 1

/* As instructed in the softfloat documentation, we do not expose the original
 * softfloat.h header. Instead, we create our own. This has the great upside of
 * letting us declare functions that takes native types (i.e. float, double). */

int f64_to_ui32_r_minMag(double x, int i);
double ui32_to_f64(int i);

int f64_eq(double x, double y);
int f64_le(double x, double y);
int f64_lt(double x, double y);
double f64_add(double x, double y);
double f64_sub(double x, double y);
double f64_mul(double x, double y);
double f64_div(double x, double y);

#endif
