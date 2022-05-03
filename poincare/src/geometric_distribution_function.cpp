#include <poincare/geometric_distribution_function.h>
#include <poincare/geometric_distribution.h>
#include <poincare/simplification_helper.h>
#include <assert.h>

namespace Poincare {

Expression GeometricDistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return GeometricDistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression GeometricDistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression p = childAtIndex(1);

  // Check p
  bool pOK = false;
  bool couldCheckP = GeometricDistribution::ExpressionPIsOK(&pOK, p, context);
  if (!couldCheckP) {
    return *this;
  }
  if (!pOK) {
    return replaceWithUndefinedInPlace();
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}

}
