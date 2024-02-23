#include <poincare/cdf_method.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace Poincare {

OExpression CDFMethod::shallowReduce(OExpression* abscissae,
                                     const Distribution* distribution,
                                     OExpression* parameters,
                                     ReductionContext reductionContext,
                                     OExpression* expression) const {
  OExpression x = abscissae[0];

  if (x.type() == ExpressionNode::Type::Infinity) {
    if (x.isPositive(reductionContext.context()) == TrinaryBoolean::False) {
      OExpression result = Rational::Builder(0);
      expression->replaceWithInPlace(result);
      return result;
    } else {
      OExpression result = Rational::Builder(1);
      expression->replaceWithInPlace(result);
      return result;
    }
  }

  return *expression;
}

}  // namespace Poincare
