#include <poincare/list_sample_standard_deviation.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/list_standard_deviation.h>
#include <poincare/list_sum.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/statistics_dataset.h>
#include <poincare/square_root.h>

namespace Poincare {

template<int U>
int ListSampleStandardDeviationNode<U>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListSampleStandardDeviation::k_functionName);
}

template<int U>
Layout ListSampleStandardDeviationNode<U>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListSampleStandardDeviation(this), floatDisplayMode, numberOfSignificantDigits, ListSampleStandardDeviation::k_functionName);
}

template<int U>
Expression ListSampleStandardDeviationNode<U>::shallowReduce(ReductionContext reductionContext) {
  return ListSampleStandardDeviation(this).shallowReduce(reductionContext);
}

template<int U>
template<typename T> Evaluation<T> ListSampleStandardDeviationNode<U>::templatedApproximate(ApproximationContext approximationContext) const {
  ListComplex<T> evaluationArray[2];
  StatisticsDataset<T> dataset = StatisticsDataset<T>::BuildFromChildren(this, approximationContext, evaluationArray);
  if (dataset.isUndefined()) {
    return Complex<T>::Undefined();
  }
  return Complex<T>::Builder(dataset.sampleStandardDeviation());
}

Expression ListSampleStandardDeviation::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  int n = numberOfChildren();
  assert(n <= 2);
  Expression children[2];
  for (int i = 0; i < n; i++) {
    children[i] = childAtIndex(i);
    if (children[i].type() != ExpressionNode::Type::List || children[i].numberOfChildren() == 0) {
      return replaceWithUndefinedInPlace();
    }
  }
  // Sample sttdev = sttdev * sqrt(weights / weights - 1)
  Expression correctionFactor;
  if (n == 1) {
    int totalWeight = children[0].numberOfChildren();
    if (totalWeight == 1) {
      // escape this case here since it's not handle by the formula above.
      return replaceWithUndefinedInPlace();
    }
    Rational fraction = Rational::Builder(totalWeight, totalWeight - 1);
    correctionFactor = Power::Builder(fraction, Rational::Builder(1, 2));
  } else {
    assert(n == 2);
    ListSum totalWeight = ListSum::Builder(children[1].clone());
    Addition a = Addition::Builder(totalWeight, Rational::Builder(-1, 1)); // n - 1
    totalWeight.shallowReduce(reductionContext);
    Power p = Power::Builder(a, Rational::Builder(-1, 1)); // 1/(n-1)
    a.shallowReduce(reductionContext);
    Addition q = Addition::Builder(Rational::Builder(1, 1), p); // 1 + 1/(n-1) = n/(n-1)
    p.shallowReduce(reductionContext);
    correctionFactor = Power::Builder(q, Rational::Builder(1, 2)); // sqrt(n/(n-1))
    q.shallowReduce(reductionContext);
  }
  ListStandardDeviation sttdev = n == 1 ? ListStandardDeviation::Builder(children[0]) : ListStandardDeviation::Builder(children[0], children[1]);
  Multiplication m = Multiplication::Builder(correctionFactor, sttdev);
  sttdev.shallowReduce(reductionContext);
  correctionFactor.shallowReduce(reductionContext);
  replaceWithInPlace(m);
  return m.shallowReduce(reductionContext);
}

template Evaluation<float> ListSampleStandardDeviationNode<1>::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<float> ListSampleStandardDeviationNode<2>::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListSampleStandardDeviationNode<1>::templatedApproximate<double>(ApproximationContext approximationContext) const;
template Evaluation<double> ListSampleStandardDeviationNode<2>::templatedApproximate<double>(ApproximationContext approximationContext) const;

template class ListSampleStandardDeviationNode<1>;
template class ListSampleStandardDeviationNode<2>;

}
