#include <poincare/list_standard_deviation.h>
#include <poincare/layout_helper.h>
#include <poincare/list_variance.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/statistics_dataset.h>

namespace Poincare {

template<int U>
int ListStandardDeviationNode<U>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListStandardDeviation::k_functionName);
}

template<int U>
Layout ListStandardDeviationNode<U>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListStandardDeviation(this), floatDisplayMode, numberOfSignificantDigits, ListStandardDeviation::k_functionName);
}

template<int U>
Expression ListStandardDeviationNode<U>::shallowReduce(const ReductionContext& reductionContext) {
  return ListStandardDeviation(this).shallowReduce(reductionContext);
}

template<int U>
template<typename T> Evaluation<T> ListStandardDeviationNode<U>::templatedApproximate(const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.standardDeviation());
}

Expression ListStandardDeviation::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  int n = numberOfChildren();
  assert(n <= 2);
  Expression children[2];
  if (!SimplificationHelper::getChildrenIfNonEmptyList(*this, children)) {
    return replaceWithUndefinedInPlace();
  }
  ListVariance var = n == 1 ? ListVariance::Builder(children[0]) : ListVariance::Builder(children[0], children[1]);
  Power sqrt = Power::Builder(var, Rational::Builder(1, 2));
  var.shallowReduce(reductionContext);
  replaceWithInPlace(sqrt);
  return sqrt.shallowReduce(reductionContext);
}

template Evaluation<float> ListStandardDeviationNode<1>::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<float> ListStandardDeviationNode<2>::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListStandardDeviationNode<1>::templatedApproximate<double>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListStandardDeviationNode<2>::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

template class ListStandardDeviationNode<1>;
template class ListStandardDeviationNode<2>;

}
