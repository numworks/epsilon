#include <poincare/layout.h>
#include <poincare/list.h>
#include <poincare/list_maximum.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

int ListMaximumNode::numberOfChildren() const {
  return ListMaximum::s_functionHelper.numberOfChildren();
}

size_t ListMaximumNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ListMaximum::s_functionHelper.aliasesList().mainAlias());
}

OExpression ListMaximumNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListMaximum(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListMaximumNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  ExpressionNode* child = childAtIndex(0);
  if (child->otype() != ExpressionNode::Type::OList) {
    return Complex<T>::Undefined();
  }
  return static_cast<ListNode*>(child)->extremumApproximation<T>(
      approximationContext, false);
}

OExpression ListMaximum::shallowReduce(ReductionContext reductionContext) {
  OExpression child = childAtIndex(0);
  if (child.otype() != ExpressionNode::Type::OList ||
      child.numberOfChildren() == 0 ||
      recursivelyMatches(OExpression::IsUndefined, nullptr)) {
    return replaceWithUndefinedInPlace();
  }
  OExpression result =
      static_cast<OList&>(child).extremum(reductionContext, false);
  if (result.isUndefined()) {
    // Let approximation handle this
    return *this;
  }
  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> ListMaximumNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMaximumNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
