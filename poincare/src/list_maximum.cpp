#include <poincare/list_maximum.h>
#include <poincare/list.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

int ListMaximumNode::numberOfChildren() const {
  return ListMaximum::s_functionHelper.numberOfChildren();
}

int ListMaximumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMaximum::s_functionHelper.aliasesList().mainAlias());
}

Layout ListMaximumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ListMaximum(this), floatDisplayMode, numberOfSignificantDigits, ListMaximum::s_functionHelper.aliasesList().mainAlias(), context);
}

Expression ListMaximumNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListMaximum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMaximumNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }
  return static_cast<ListNode *>(child)->extremumApproximation<T>(approximationContext, false);
}

Expression ListMaximum::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }
  Expression result = static_cast<List &>(child).extremum(reductionContext, false);
  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> ListMaximumNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMaximumNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
