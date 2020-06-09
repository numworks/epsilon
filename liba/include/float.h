#ifndef LIBA_FLOAT_H
#define LIBA_FLOAT_H

#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__

#define FLT_MAX 1E+37f
#define FLT_MIN 1E-37f
/* TODO Redefine FLT_EPSILON to comply with the definition :
 * FLT_EPSILON is the difference between 1 and the very next floating point
 * number. */
#define FLT_EPSILON 1E-5f
#define DBL_MAX 1.79769313486231571e+308
#define DBL_MIN 2.22507385850720138e-308
#define DBL_EPSILON 2.2204460492503131e-16

#define LDBL_MANT_DIG (-1)

#endif
