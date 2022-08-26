#include <poincare/list_minimum.h>
#include <poincare/list.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

int ListMinimumNode::numberOfChildren() const {
  return ListMinimum::s_functionHelper.numberOfChildren();
}

int ListMinimumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMinimum::s_functionHelper.aliasesList().mainAlias());
}

Layout ListMinimumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ListMinimum(this), floatDisplayMode, numberOfSignificantDigits, ListMinimum::s_functionHelper.aliasesList().mainAlias(), context);
}

Expression ListMinimumNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListMinimum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMinimumNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }
  return static_cast<ListNode *>(child)->extremumApproximation<T>(approximationContext, true);
}

Expression ListMinimum::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  Expression result = static_cast<List &>(child).extremum(reductionContext, true);
  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> ListMinimumNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMinimumNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
