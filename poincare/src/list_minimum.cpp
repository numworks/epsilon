#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/list_minimum.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

int ListMinimumNode::numberOfChildren() const {
  return ListMinimum::s_functionHelper.numberOfChildren();
}

size_t ListMinimumNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListMinimum::s_functionHelper.aliasesList().mainAlias());
}

OExpression ListMinimumNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListMinimum(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListMinimumNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  ExpressionNode* child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }
  return static_cast<ListNode*>(child)->extremumApproximation<T>(
      approximationContext, true);
}

OExpression ListMinimum::shallowReduce(ReductionContext reductionContext) {
  OExpression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List ||
      child.numberOfChildren() == 0 ||
      recursivelyMatches(OExpression::IsUndefined, nullptr)) {
    return replaceWithUndefinedInPlace();
  }
  OExpression result =
      static_cast<List&>(child).extremum(reductionContext, true);
  if (result.isUndefined()) {
    // Let approximation handle this
    return *this;
  }
  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> ListMinimumNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMinimumNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
