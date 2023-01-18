#include <poincare/list_sample_standard_deviation.h>
#include <poincare/addition.h>
#include <poincare/list_standard_deviation.h>
#include <poincare/list_sum.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/simplification_helper.h>
#include <poincare/statistics_dataset.h>
#include <poincare/square_root.h>

namespace Poincare {

Expression ListSampleStandardDeviationNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListSampleStandardDeviation(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListSampleStandardDeviationNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.sampleStandardDeviation());
}

Expression ListSampleStandardDeviation::shallowReduce(ReductionContext reductionContext) {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  Expression children[2];
  if (!static_cast<ListFunctionWithOneOrTwoParametersNode *>(node())->getChildrenIfNonEmptyList(children)) {
    return replaceWithUndefinedInPlace();
  }
  // Sample sttdev = sttdev * sqrt(weights / weights - 1)
  ListSum totalWeight = ListSum::Builder(children[1].clone());
  Addition a = Addition::Builder(totalWeight, Rational::Builder(-1, 1)); // n - 1
  totalWeight.shallowReduce(reductionContext);
  Power p = Power::Builder(a, Rational::Builder(-1, 1)); // 1/(n-1)
  a.shallowReduce(reductionContext);
  Addition q = Addition::Builder(Rational::Builder(1, 1), p); // 1 + 1/(n-1) = n/(n-1)
  p.shallowReduce(reductionContext);
  Expression correctionFactor = Power::Builder(q, Rational::Builder(1, 2)); // sqrt(n/(n-1))
  q.shallowReduce(reductionContext);
  ListStandardDeviation sttdev = ListStandardDeviation::Builder(children[0], children[1]);
  Multiplication m = Multiplication::Builder(correctionFactor, sttdev);
  sttdev.shallowReduce(reductionContext);
  correctionFactor.shallowReduce(reductionContext);
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

template Evaluation<float> ListSampleStandardDeviationNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListSampleStandardDeviationNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
