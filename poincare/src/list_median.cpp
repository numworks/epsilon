#include <poincare/list_median.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <poincare/statistics_dataset.h>
#include <float.h>

namespace Poincare {

Expression ListMedianNode::shallowReduce(const ReductionContext& reductionContext) {
  return ListMedian(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMedianNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.median());
}

Expression ListMedian::shallowReduce(ReductionContext reductionContext) {
  ApproximationContext approximationContext(reductionContext, true);
  for (int k = 0; k < numberOfChildren(); k++) {
    Expression listChild = childAtIndex(k);
    int n = listChild.numberOfChildren();
    for (int i = 0; i < n; i++) {
      if (listChild.childAtIndex(i).isUndefined()) {
        return replaceWithUndefinedInPlace();
      }
      if (std::isnan(listChild.childAtIndex(i).node()->approximate(0.0f, approximationContext).toScalar())) {
        /* One of the children can't be approximated for now, but could be
         * later: let approximation handle this */
        return *this;
      }
    }
  }
  ListComplex<double> evaluationArray[2];
  StatisticsDataset<double> dataset = StatisticsDataset<double>::BuildFromChildren(node(), approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return replaceWithUndefinedInPlace();
  }
  int upperMedianIndex;
  int lowerMedianIndex = dataset.medianIndex(&upperMedianIndex);
  if (lowerMedianIndex < 0) {
    return replaceWithUndefinedInPlace();
  }
  if (upperMedianIndex == lowerMedianIndex) {
    Expression e = childAtIndex(0).childAtIndex(upperMedianIndex);
    replaceWithInPlace(e);
    return e;
  }
  Expression a = childAtIndex(0).childAtIndex(upperMedianIndex), b = childAtIndex(0).childAtIndex(lowerMedianIndex);
  Addition sum = Addition::Builder(a, b);
  Division div = Division::Builder(sum, Rational::Builder(2));
  sum.shallowReduce(reductionContext);
  replaceWithInPlace(div);
  return div.shallowReduce(reductionContext);
}

template Evaluation<float> ListMedianNode::templatedApproximate<float>(const ApproximationContext& approximationContext) const;
template Evaluation<double> ListMedianNode::templatedApproximate<double>(const ApproximationContext& approximationContext) const;

}
