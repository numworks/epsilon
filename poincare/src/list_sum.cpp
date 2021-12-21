#include <poincare/list_sum.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

const Expression::FunctionHelper ListSum::s_functionHelper;

int ListSumNode::numberOfChildren() const {
  return ListSum::s_functionHelper.numberOfChildren();
}

int ListSumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSum::s_functionHelper.name());
}

Layout ListSumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListSum(this), floatDisplayMode, numberOfSignificantDigits, ListSum::s_functionHelper.name());
}

Expression ListSumNode::shallowReduce(ReductionContext reductionContext) {
  return ListSum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListSumNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  Evaluation<T> result = Complex<T>::Builder(0);
  int n = child->numberOfChildren();
  for (int i = 0; i < n; i++) {
    result = Evaluation<T>::Sum(result, child->childAtIndex(i)->approximate(static_cast<T>(0), approximationContext), approximationContext.complexFormat());
  }
  return result;
}

Expression ListSum::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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

}
