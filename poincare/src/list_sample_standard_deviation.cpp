#include <poincare/list_sample_standard_deviation.h>
#include <poincare/layout_helper.h>
#include <poincare/list_variance.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_root.h>

namespace Poincare {

const Expression::FunctionHelper ListSampleStandardDeviation::s_functionHelper;

int ListSampleStandardDeviationNode::numberOfChildren() const {
  return ListSampleStandardDeviation::s_functionHelper.numberOfChildren();
}

int ListSampleStandardDeviationNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSampleStandardDeviation::s_functionHelper.name());
}

Layout ListSampleStandardDeviationNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListSampleStandardDeviation(this), floatDisplayMode, numberOfSignificantDigits, ListSampleStandardDeviation::s_functionHelper.name());
}

Expression ListSampleStandardDeviationNode::shallowReduce(ReductionContext reductionContext) {
  return ListSampleStandardDeviation(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListSampleStandardDeviationNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  Evaluation<T> variance = ListVarianceNode::VarianceOfListNode<T>(static_cast<ListNode *>(child), approximationContext);
  int n = child->numberOfChildren();
  Evaluation<T> unbiased = MultiplicationNode::Compute<T>(variance, Complex<T>::Builder(n / (n - static_cast<T>(1))), approximationContext.complexFormat());
  if (unbiased.type() != EvaluationNode<T>::Type::Complex) {
    return Complex<T>::Undefined();
  }
  return SquareRootNode::computeOnComplex<T>(unbiased.complexAtIndex(0), approximationContext.complexFormat(), approximationContext.angleUnit());
}

Expression ListSampleStandardDeviation::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  int n = child.numberOfChildren();
  if (child.type() != ExpressionNode::Type::List || n <= 1) {
    return replaceWithUndefinedInPlace();
  }

  ListVariance var = ListVariance::Builder(child);
  Multiplication m = Multiplication::Builder(Rational::Builder(n, n - 1), var);
  var.shallowReduce(reductionContext);
  Power sqrt = Power::Builder(m, Rational::Builder(1, 2));
  m.shallowReduce(reductionContext);
  replaceWithInPlace(sqrt);
  return sqrt.shallowReduce(reductionContext);
}

template Evaluation<float> ListSampleStandardDeviationNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListSampleStandardDeviationNode::templatedApproximate<double>(ApproximationContext approximationContext) const;

}
