#include <poincare/list_standard_deviation.h>
#include <poincare/layout_helper.h>
#include <poincare/list_variance.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_root.h>

namespace Poincare {

const Expression::FunctionHelper ListStandardDeviation::s_functionHelper;

int ListStandardDeviationNode::numberOfChildren() const {
  return ListStandardDeviation::s_functionHelper.numberOfChildren();
}

int ListStandardDeviationNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListStandardDeviation::s_functionHelper.name());
}

Layout ListStandardDeviationNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListStandardDeviation(this), floatDisplayMode, numberOfSignificantDigits, ListStandardDeviation::s_functionHelper.name());
}

Expression ListStandardDeviationNode::shallowReduce(ReductionContext reductionContext) {
  return ListStandardDeviation(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListStandardDeviationNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  Evaluation<T> variance = static_cast<ListNode *>(child)->variance<T>(approximationContext);
  if (variance.type() != EvaluationNode<T>::Type::Complex) {
    return Complex<T>::Undefined();
  }
  return SquareRootNode::computeOnComplex<T>(variance.complexAtIndex(0), approximationContext.complexFormat(), approximationContext.angleUnit());
}

Expression ListStandardDeviation::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  ListVariance var = ListVariance::Builder(child);
  Power sqrt = Power::Builder(var, Rational::Builder(1, 2));
  var.shallowReduce(reductionContext);
  replaceWithInPlace(sqrt);
  return sqrt.shallowReduce(reductionContext);
}

template Evaluation<float> ListStandardDeviationNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListStandardDeviationNode::templatedApproximate<double>(ApproximationContext approximationContext) const;


}
