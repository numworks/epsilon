#ifndef POINCARE_SIMPLIFY_PRODUCT_ZERO_H
#define POINCARE_SIMPLIFY_PRODUCT_ZERO_H

#include "simplify.h"

// a*b*c*0 -> 0

Expression * SimplifyProductZero(Expression * e);

#endif
