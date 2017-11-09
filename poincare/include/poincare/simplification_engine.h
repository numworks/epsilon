#ifndef POINCARE_SIMPLIFICATION_ENGINE_H
#define POINCARE_SIMPLIFICATION_ENGINE_H

#include <poincare/expression.h>

namespace Poincare {

class SimplificationEngine {
public:
  static Expression * map(Expression * e, Context & context, Expression::AngleUnit angleUnit);

};

}

#endif
