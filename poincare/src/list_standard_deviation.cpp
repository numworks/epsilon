#include <poincare/list_standard_deviation.h>
#include <poincare/list_variance.h>
#include <poincare/power.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/statistics_dataset.h>

namespace Poincare {

Expression ListStandardDeviationNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListStandardDeviation(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListStandardDeviationNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.standardDeviation());
}

Expression ListStandardDeviation::shallowReduce(ReductionContext reductionContext) {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  Expression children[2];
  if (!static_cast<ListFunctionWithOneOrTwoParametersNode *>(node())->getChildrenIfNonEmptyList(children)) {
    return replaceWithUndefinedInPlace();
  }
  ListVariance var = ListVariance::Builder(children[0], children[1]);
  Power sqrt = Power::Builder(var, Rational::Builder(1, 2));
  var.shallowReduce(reductionContext);
  replaceWithInPlace(sqrt);
  return sqrt.shallowReduce(reductionContext);
}

template Evaluation<float> ListStandardDeviationNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListStandardDeviationNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
