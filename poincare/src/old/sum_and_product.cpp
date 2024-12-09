#include <poincare/old/decimal.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/sum_and_product.h>
#include <poincare/old/undefined.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

OExpression SumAndProductNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return SumAndProduct(this).shallowReduce(reductionContext);
}

OExpression SumAndProduct::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  if (otype() == ExpressionNode::Type::Sum &&
      Poincare::Preferences::SharedPreferences()->examMode().forbidSum()) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

}  // namespace Poincare
