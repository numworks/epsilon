#include <poincare/cdf_range_method.h>
#include <poincare/distribution_dispatcher.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace Poincare {

Expression CDFRangeMethod::shallowReduce(Expression * abscissae, const Distribution * distribution, Expression * parameters, ReductionContext reductionContext, Expression * expression) const {
  Expression x = abscissae[0];
  Expression y = abscissae[1];

  if (x.type() == ExpressionNode::Type::Infinity && x.isPositive(reductionContext.context()) == TrinaryBoolean::False) {
    if (y.type() == ExpressionNode::Type::Infinity) {
      Expression result = Rational::Builder(y.isPositive(reductionContext.context()) == TrinaryBoolean::True);
      expression->replaceWithInPlace(result);
      return result;
    }
    // TODO: return CDF of the same distributions with the same parameters
    // tcdfrange(-inf, 4, 5) => tcdf(4, 5)
  }

  return *expression;
}

}
