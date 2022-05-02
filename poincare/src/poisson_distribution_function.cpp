#include <poincare/poisson_distribution_function.h>
#include <poincare/poisson_distribution.h>
#include <poincare/simplification_helper.h>
#include <assert.h>

namespace Poincare {

Expression PoissonDistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return PoissonDistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression PoissonDistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression lambda = childAtIndex(1);

  // Check lambda
  bool lambdaOK = false;
  bool couldCheckLambda = PoissonDistribution::ExpressionLambdaIsOK(&lambdaOK, lambda, context);
  if (!couldCheckLambda) {
    return *this;
  }
  if (!lambdaOK) {
    return replaceWithUndefinedInPlace();
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}

}
