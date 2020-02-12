#include <poincare/binomial_distribution_function.h>
#include <poincare/binomial_distribution.h>
#include <assert.h>

namespace Poincare {

Expression BinomialDistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return BinomialDistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression BinomialDistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }

  Expression n = childAtIndex(1);
  Expression p = childAtIndex(2);

  // Check mu and var
  bool nAndPOK = false;
  bool couldCheckNAndP = BinomialDistribution::ExpressionParametersAreOK(&nAndPOK, n, p, context);
  if (!couldCheckNAndP) {
    return *this;
  }
  if (!nAndPOK) {
    return replaceWithUndefinedInPlace();
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}

}
