#include "cdf_method.h"

#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace PoincareJ {

bool CDFMethod::shallowReduce(const Tree** abscissae,
                              const Distribution* distribution,
                              const Tree** parameters, Tree* expression) const {
  Expression x = abscissae[0];

  if (x.type() == ExpressionNode::Type::Infinity) {
    if (x.isPositive(reductionContext.context()) == TrinaryBoolean::False) {
      Expression result = Rational::Builder(0);
      expression->replaceWithInPlace(result);
      return result;
    } else {
      Expression result = Rational::Builder(1);
      expression->replaceWithInPlace(result);
      return result;
    }
  }

  return *expression;
}

}  // namespace PoincareJ
