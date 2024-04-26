#include <poincare/old/list_complex.h>
#include <poincare/old/list_mean.h>
#include <poincare/old/list_sum.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/statistics_dataset.h>
#include <poincare/old/undefined.h>

namespace Poincare {

OExpression ListMeanNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListMean(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListMeanNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(
      this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.mean());
}

OExpression ListMean::shallowReduce(ReductionContext reductionContext) {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  OExpression children[2];
  if (!static_cast<ListFunctionWithOneOrTwoParametersNode*>(node())
           ->getChildrenIfNonEmptyList(children)) {
    return replaceWithUndefinedInPlace();
  }
  // All weights need to be positive.
  bool allWeightsArePositive = true;
  int childrenNumber = children[1].numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    TrinaryBoolean childIsPositive =
        children[1].childAtIndex(i).isPositive(reductionContext.context());
    if (childIsPositive == TrinaryBoolean::False) {
      // If at least one child is negative, return undef
      return replaceWithUndefinedInPlace();
    }
    if (childIsPositive == TrinaryBoolean::Unknown) {
      allWeightsArePositive = false;
    }
  }
  if (!allWeightsArePositive) {
    // Could not find a negative but some children have unknown sign
    return *this;
  }
  OExpression listToSum =
      Multiplication::Builder(children[0], children[1].clone());
  ListSum sum = ListSum::Builder(listToSum);
  listToSum.shallowReduce(reductionContext);
  ListSum sumOfWeights = ListSum::Builder(children[1]);
  OExpression inverseOfTotalWeights =
      Power::Builder(sumOfWeights, Rational::Builder(-1));
  sumOfWeights.shallowReduce(reductionContext);
  Multiplication result = Multiplication::Builder(sum, inverseOfTotalWeights);
  sum.shallowReduce(reductionContext);
  inverseOfTotalWeights.shallowReduce(reductionContext);
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

template Evaluation<float> ListMeanNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMeanNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;
}  // namespace Poincare
