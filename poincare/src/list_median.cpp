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

template<int U>
int ListMedianNode<U>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMedian::k_functionName);
}

template<int U>
Layout ListMedianNode<U>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListMedian(this), floatDisplayMode, numberOfSignificantDigits, ListMedian::k_functionName);
}

template<int U>
Expression ListMedianNode<U>::shallowReduce(const ReductionContext& reductionContext) {
  return ListMedian(this).shallowReduce(reductionContext);
}

template<int U>
template<typename T> Evaluation<T> ListMedianNode<U>::templatedApproximate(ApproximationContext approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.median());
}

Expression ListMedian::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  ExpressionNode::ApproximationContext approximationContext(reductionContext, true);
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

template Evaluation<float> ListMedianNode<1>::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<float> ListMedianNode<2>::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListMedianNode<1>::templatedApproximate<double>(ApproximationContext approximationContext) const;
template Evaluation<double> ListMedianNode<2>::templatedApproximate<double>(ApproximationContext approximationContext) const;

template class ListMedianNode<1>;
template class ListMedianNode<2>;

}
