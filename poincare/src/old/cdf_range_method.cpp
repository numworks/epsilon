#include <poincare/old/cdf_range_method.h>
#include <poincare/old/distribution_dispatcher.h>
#include <poincare/old/infinity.h>
#include <poincare/old/integer.h>
#include <poincare/old/rational.h>

namespace Poincare {

OExpression CDFRangeMethod::shallowReduce(OExpression* abscissae,
                                          const Distribution* distribution,
                                          OExpression* parameters,
                                          ReductionContext reductionContext,
                                          OExpression* expression) const {
  OExpression x = abscissae[0];
  OExpression y = abscissae[1];

  if (x.otype() == ExpressionNode::Type::Infinity &&
      x.isPositive(reductionContext.context()) == TrinaryBoolean::False) {
    if (y.otype() == ExpressionNode::Type::Infinity) {
      OExpression result = Rational::Builder(
          y.isPositive(reductionContext.context()) == TrinaryBoolean::True);
      expression->replaceWithInPlace(result);
      return result;
    }
    /* TODO: return CDF of the same distributions with the same parameters
     * tcdfrange(-inf, 4, 5) => tcdf(4, 5) */
  }

  return *expression;
}

}  // namespace Poincare
