#include <poincare/binomial_distribution_function.h>
#include <poincare/binomial_distribution.h>
#include <poincare/simplification_helper.h>
#include <assert.h>

namespace Poincare {

Expression BinomialDistributionFunctionNode::shallowReduce(const ReductionContext& reductionContext) {
  return BinomialDistributionFunction(this).shallowReduce(reductionContext);
}

Expression BinomialDistributionFunction::shallowReduce(ExpressionNode::ReductionContext reductionContext, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }

  Expression n = childAtIndex(1);
  Expression p = childAtIndex(2);

  // Check mu and var
  bool nAndPOK = false;
  bool couldCheckNAndP = BinomialDistribution::ExpressionParametersAreOK(&nAndPOK, n, p, reductionContext.context());
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
