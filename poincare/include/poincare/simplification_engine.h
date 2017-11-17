#ifndef POINCARE_SIMPLIFICATION_ENGINE_H
#define POINCARE_SIMPLIFICATION_ENGINE_H

#if MATRIX_EXACT_REDUCING

#include <poincare/expression.h>

namespace Poincare {

class SimplificationEngine {
public:
  static Expression * map(Expression * e, Context & context, Expression::AngleUnit angleUnit);

};

}

#endif

#endif
