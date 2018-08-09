#ifndef POINCARE_SIMPLIFICATION_HELPER_H
#define POINCARE_SIMPLIFICATION_HELPER_H

#if MATRIX_EXACT_REDUCING

#include <poincare/expression.h>

namespace Poincare {

namespace SimplificationHelper {
  static Expression Map(Expression e, Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif

#endif
