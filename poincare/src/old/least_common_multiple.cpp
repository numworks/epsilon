#include <poincare/layout.h>
#include <poincare/old/arithmetic.h>
#include <poincare/old/least_common_multiple.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

constexpr OExpression::FunctionHelper LeastCommonMultiple::s_functionHelper;

size_t LeastCommonMultipleNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      LeastCommonMultiple::s_functionHelper.aliasesList().mainAlias());
}

OExpression LeastCommonMultipleNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return LeastCommonMultiple(this).shallowReduce(reductionContext);
}

OExpression LeastCommonMultipleNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return LeastCommonMultiple(this).shallowBeautify(reductionContext.context());
}

OExpression LeastCommonMultiple::shallowBeautify(Context* context) {
  /* Sort children in decreasing order:
   * lcm(1,x,x^2) --> lcm(x^2,x,1)
   * lcm(1,R(2)) --> lcm(R(2),1) */
  sortChildrenInPlace(
      [](const ExpressionNode* e1, const ExpressionNode* e2) {
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      context, true, false);
  return *this;
}

OExpression LeastCommonMultiple::shallowReduce(
    ReductionContext reductionContext) {
#if 0  // TODO_PCJ: Delete this method
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
  assert(numberOfChildren() > 0);

  // Step 0: Merge children which are LCM
  mergeSameTypeChildrenInPlace();

  // Step 1: check that all children are compatible
  {
    OExpression checkChildren =
        checkChildrenAreRationalIntegersAndUpdate(reductionContext);
    if (!checkChildren.isUninitialized()) {
      return checkChildren;
    }
  }

  // Step 2: Compute LCM
  OExpression result = Arithmetic::LCM(*this);

  replaceWithInPlace(result);
  return result;
#else
  return *this;
#endif
}

}  // namespace Poincare
