#include <poincare/normal_distribution_function.h>
#include <poincare/normal_distribution.h>
#include <poincare/square_root.h>
#include <poincare/simplification_helper.h>
#include <assert.h>

namespace Poincare {

Expression NormalDistributionFunctionNode::shallowReduce(const ReductionContext& reductionContext) {
  return NormalDistributionFunction(this).shallowReduce(reductionContext);
}

Expression NormalDistributionFunction::shallowReduce(ExpressionNode::ReductionContext reductionContext, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }

  // Check mu and var
  bool muAndVarOK = false;
  bool couldCheckMuAndVar = NormalDistribution::ExpressionMuAndVarAreOK(
      &muAndVarOK,
      childAtIndex(muIndex()),
      childAtIndex(varIndex()),
      reductionContext.context());
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
