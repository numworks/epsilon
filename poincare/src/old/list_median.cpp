#include <float.h>
#include <poincare/layout.h>
#include <poincare/old/addition.h>
#include <poincare/old/division.h>
#include <poincare/old/list.h>
#include <poincare/old/list_median.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/statistics_dataset.h>

namespace Poincare {

OExpression ListMedianNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ListMedian(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> ListMedianNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(
      this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.median());
}

OExpression ListMedian::shallowReduce(ReductionContext reductionContext) {
  ApproximationContext approximationContext(reductionContext, true);
  for (int k = 0; k < numberOfChildren(); k++) {
    OExpression listChild = childAtIndex(k);
    int n = listChild.numberOfChildren();
    for (int i = 0; i < n; i++) {
      if (listChild.childAtIndex(i).isUndefined()) {
        return replaceWithUndefinedInPlace();
      }
      if (std::isnan(listChild.childAtIndex(i)
                         .node()
                         ->approximate(0.0f, approximationContext)
                         .toScalar())) {
        /* One of the children can't be approximated for now, but could be
         * later: let approximation handle this */
        return *this;
      }
    }
  }
  ListComplex<double> evaluationArray[2];
  StatisticsDataset<double> dataset =
      StatisticsDataset<double>::BuildFromChildren(node(), approximationContext,
                                                   evaluationArray);
  if (dataset.isUndefined()) {
    return replaceWithUndefinedInPlace();
  }
  int upperMedianIndex;
  int lowerMedianIndex = dataset.medianIndex(&upperMedianIndex);
  if (lowerMedianIndex < 0) {
    return replaceWithUndefinedInPlace();
  }
  if (upperMedianIndex == lowerMedianIndex) {
    OExpression e = childAtIndex(0).childAtIndex(upperMedianIndex);
    replaceWithInPlace(e);
    return e;
  }
  OExpression a = childAtIndex(0).childAtIndex(upperMedianIndex),
              b = childAtIndex(0).childAtIndex(lowerMedianIndex);
  Addition sum = Addition::Builder(a, b);
  Division div = Division::Builder(sum, Rational::Builder(2));
  sum.shallowReduce(reductionContext);
  replaceWithInPlace(div);
  return div.shallowReduce(reductionContext);
}

template Evaluation<float> ListMedianNode::templatedApproximate<float>(
    const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMedianNode::templatedApproximate<double>(
    const ApproximationContext& approximationContext) const;

}  // namespace Poincare
