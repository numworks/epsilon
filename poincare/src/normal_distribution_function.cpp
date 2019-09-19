#include <poincare/normal_distribution_function.h>
#include <poincare/normal_distribution.h>
#include <assert.h>

namespace Poincare {

Expression NormalDistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return NormalDistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression NormalDistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }

  Expression mu = childAtIndex(muIndex());
  Expression var = childAtIndex(varIndex());

  // Check mu and var
  bool muAndVarOK = false;
  bool couldCheckMuAndVar = NormalDistribution::ExpressionParametersAreOK(&muAndVarOK, mu, var, context);
  if (!couldCheckMuAndVar) {
    return *this;
  }
  if (!muAndVarOK) {
    return replaceWithUndefinedInPlace();
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}

}
