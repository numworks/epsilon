#include <poincare/addition.h>
#include <poincare/list_mean.h>
#include <poincare/list_variance.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/simplification_helper.h>
#include <poincare/statistics_dataset.h>
#include <poincare/subtraction.h>

namespace Poincare {

Expression ListVarianceNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListVariance(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListVarianceNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(
      this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.variance());
}

Expression ListVariance::shallowReduce(ReductionContext reductionContext) {
  /* var(L) = mean(L^2) - mean(L)^2 */
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  Expression children[2];
  if (!static_cast<ListFunctionWithOneOrTwoParametersNode*>(node())
           ->getChildrenIfNonEmptyList(children)) {
    return replaceWithUndefinedInPlace();
  }
  Expression m = ListMean::Builder(children[0].clone(), children[1].clone());
  Power m2 = Power::Builder(m, Rational::Builder(2));
  m.shallowReduce(reductionContext);
  Power l2 = Power::Builder(children[0], Rational::Builder(2));
  Expression ml2 = ListMean::Builder(l2, children[1]);
  l2.shallowReduce(reductionContext);
  Subtraction s = Subtraction::Builder(ml2, m2);
  ml2.shallowReduce(reductionContext);
  m2.shallowReduce(reductionContext);
  replaceWithInPlace(s);
  return s.shallowReduce(reductionContext);
}

template Evaluation<float> ListVarianceNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListVarianceNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;
}  // namespace Poincare
