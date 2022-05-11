#include <poincare/cdf_range_method.h>
#include <poincare/distribution_dispatcher.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace Poincare {

Expression CDFRangeMethod::shallowReduce(Expression * abscissae, const Distribution * distribution, Expression * parameters, ExpressionNode::ReductionContext reductionContext, Expression * expression) const {
  Expression x = abscissae[0];
  Expression y = abscissae[1];

  if (x.type() == ExpressionNode::Type::Infinity && x.sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
    if (y.type() == ExpressionNode::Type::Infinity) {
      if (y.sign(reductionContext.context()) == ExpressionNode::Sign::Negative) {
        Expression result = Rational::Builder(0);
        expression->replaceWithInPlace(result);
        return result;
      } else {
        Expression result = Rational::Builder(1);
        expression->replaceWithInPlace(result);
        return result;
      }
    }
    // TODO: return CDF of the same distributions with the same arguments but x
    // Expression result = DistributionDispatcher::Builder(expression->children[1:], distribution->type(), DistributionMethod::CDF);
    // expression->replaceWithInPlace(result);
    // return result;
  }

  if (x.type() != ExpressionNode::Type::Rational) {
    return *expression;
  }

  return *expression;
}

}
