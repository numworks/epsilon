#include <poincare/list_sum.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

int ListSumNode::numberOfChildren() const {
  return ListSum::s_functionHelper.numberOfChildren();
}

int ListSumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSum::s_functionHelper.aliasesList().mainAlias());
}

Layout ListSumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ListSum(this), floatDisplayMode, numberOfSignificantDigits, ListSum::s_functionHelper.aliasesList().mainAlias(), context);
}

Expression ListSumNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListSum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListSumNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  return static_cast<ListNode *>(child)->sumOfElements<T>(approximationContext);
}

Expression ListSum::shallowReduce(ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  int n = child.numberOfChildren();
  Addition sum = Addition::Builder(Rational::Builder(0));
  for (int i = 0; i < n; i++) {
    sum.addChildAtIndexInPlace(child.childAtIndex(i), i, i);
  }
  replaceWithInPlace(sum);
  return sum.shallowReduce(reductionContext);
}

template Evaluation<float> ListSumNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListSumNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
