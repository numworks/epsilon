#include <poincare/normal_distribution_function.h>
#include <poincare/normal_distribution.h>
#include <poincare/square_root.h>
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
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }

  // Check mu and var
  bool muAndVarOK = false;
  bool couldCheckMuAndVar = NormalDistribution::ExpressionMuAndVarAreOK(
      &muAndVarOK,
      childAtIndex(muIndex()),
      childAtIndex(varIndex()),
      context);
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
