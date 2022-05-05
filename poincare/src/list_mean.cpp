#include <poincare/list_mean.h>
#include <poincare/layout_helper.h>
#include <poincare/list_sum.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

const Expression::FunctionHelper ListMean::s_functionHelper;

int ListMeanNode::numberOfChildren() const {
  return ListMean::s_functionHelper.numberOfChildren();
}

int ListMeanNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMean::s_functionHelper.name());
}

Layout ListMeanNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListMean(this), floatDisplayMode, numberOfSignificantDigits, ListMean::s_functionHelper.name());
}

Expression ListMeanNode::shallowReduce(ReductionContext reductionContext) {
  return ListMean(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMeanNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  int n = child->numberOfChildren();
  if (child->type() != ExpressionNode::Type::List || n == 0) {
    return Complex<T>::Undefined();
  }
  Evaluation<T> sum = static_cast<ListNode *>(child)->sumOfElements<T>(approximationContext);
  return MultiplicationNode::Compute<T>(sum, Complex<T>::Builder(static_cast<T>(1)/n), approximationContext.complexFormat());
}

Expression ListMean::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  int n = child.numberOfChildren();
  if (child.type() != ExpressionNode::Type::List || n == 0) {
    return replaceWithUndefinedInPlace();
  }

  ListSum sum = ListSum::Builder(child);
  Multiplication result = Multiplication::Builder(sum, Rational::Builder(1, n));
  sum.shallowReduce(reductionContext);
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

template Evaluation<float> ListMeanNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListMeanNode::templatedApproximate<double>(ApproximationContext approximationContext) const;


}
