#include <poincare/list_variance.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/list_mean.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/statistics_dataset.h>
#include <poincare/subtraction.h>

namespace Poincare {

template<int U>
int ListVarianceNode<U>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListVariance::k_functionName);
}

template<int U>
Layout ListVarianceNode<U>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListVariance(this), floatDisplayMode, numberOfSignificantDigits, ListVariance::k_functionName);
}

template<int U>
Expression ListVarianceNode<U>::shallowReduce(ReductionContext reductionContext) {
  return ListVariance(this).shallowReduce(reductionContext);
}

template<int U>
template<typename T> Evaluation<T> ListVarianceNode<U>::templatedApproximate(ApproximationContext approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.variance());
}

Expression ListVariance::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  /* var(L) = mean(L^2) - mean(L)^2 */
  int n = numberOfChildren();
  assert(n <= 2);
  Expression children[2];
  for (int i = 0; i < n; i++) {
    children[i] = childAtIndex(i);
    if (children[i].type() != ExpressionNode::Type::List || children[i].numberOfChildren() == 0) {
      return replaceWithUndefinedInPlace();
    }
  }
  Expression m;
  if (n == 2) {
    m = ListMean::Builder(children[0].clone(), children[1].clone());
  } else {
    assert(n == 1);
    m = ListMean::Builder(children[0].clone());
  }
  Power m2 = Power::Builder(m, Rational::Builder(2));
  m.shallowReduce(reductionContext);
  Power l2 = Power::Builder(children[0], Rational::Builder(2));
  Expression ml2;
  if (n == 2) {
    ml2 = ListMean::Builder(l2, children[1]);
  } else {
    ml2 = ListMean::Builder(l2);
  }
  l2.shallowReduce(reductionContext);
  Subtraction s = Subtraction::Builder(ml2, m2);
  ml2.shallowReduce(reductionContext);
  m2.shallowReduce(reductionContext);
  replaceWithInPlace(s);
  return s.shallowReduce(reductionContext);
}

template Evaluation<float> ListVarianceNode<1>::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<float> ListVarianceNode<2>::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListVarianceNode<1>::templatedApproximate<double>(ApproximationContext approximationContext) const;
template Evaluation<double> ListVarianceNode<2>::templatedApproximate<double>(ApproximationContext approximationContext) const;

template class ListVarianceNode<1>;
template class ListVarianceNode<2>;
}
