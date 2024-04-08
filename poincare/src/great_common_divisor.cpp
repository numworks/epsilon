#include <poincare/arithmetic.h>
#include <poincare/great_common_divisor.h>
#include <poincare/layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

constexpr OExpression::FunctionHelper GreatCommonDivisor::s_functionHelper;

size_t GreatCommonDivisorNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      GreatCommonDivisor::s_functionHelper.aliasesList().mainAlias());
}

OExpression GreatCommonDivisorNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return GreatCommonDivisor(this).shallowReduce(reductionContext);
}

OExpression GreatCommonDivisorNode::shallowBeautify(
    const ReductionContext& reductionContext) {
  return GreatCommonDivisor(this).shallowBeautify(reductionContext.context());
}

template <typename T>
Evaluation<T> GreatCommonDivisorNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  /* TODO: distribute approx over list with Map */
  return Arithmetic::GCD<T>(*this, approximationContext);
}

OExpression GreatCommonDivisor::shallowBeautify(Context* context) {
  /* Sort children in decreasing order:
   * gcd(1,x,x^2) --> gcd(x^2,x,1)
   * gcd(1,R(2)) --> gcd(R(2),1) */
  sortChildrenInPlace(
      [](const ExpressionNode* e1, const ExpressionNode* e2) {
        return ExpressionNode::SimplificationOrder(e1, e2, false);
      },
      context, true, false);
  return *this;
}

OExpression GreatCommonDivisor::shallowReduce(
    ReductionContext reductionContext) {
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

  // Step 0: Merge children which are GCD
  mergeSameTypeChildrenInPlace();

  // Step 1: check that all children are compatible
  {
    OExpression checkChildren =
        checkChildrenAreRationalIntegersAndUpdate(reductionContext);
    if (!checkChildren.isUninitialized()) {
      return checkChildren;
    }
  }

  // Step 2: Compute GCD
  OExpression result = Arithmetic::GCD(*this);

  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> GreatCommonDivisorNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> GreatCommonDivisorNode::templatedApproximate<
    double>(const ApproximationContext& approximationContext) const;

}  // namespace Poincare
