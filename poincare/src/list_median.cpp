#include <poincare/list_median.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/list_sort.h>
#include <poincare/multiplication.h>
#include <poincare/serialization_helper.h>
#include <float.h>

namespace Poincare {

const Expression::FunctionHelper ListMedian::s_functionHelper;

int ListMedianNode::numberOfChildren() const {
  return ListMedian::s_functionHelper.numberOfChildren();
}

int ListMedianNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMedian::s_functionHelper.name());
}

Layout ListMedianNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListMedian(this), floatDisplayMode, numberOfSignificantDigits, ListMedian::s_functionHelper.name());
}

Expression ListMedianNode::shallowReduce(ReductionContext reductionContext) {
  return ListMedian(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMedianNode::templatedApproximate(ApproximationContext approximationContext) const {
  Evaluation<T> child = childAtIndex(0)->approximate(T(), approximationContext);
  if (child.type() != EvaluationNode<T>::Type::ListComplex) {
    return Complex<T>::Undefined();
  }
  ListComplex<T> list = static_cast<ListComplex<T>&>(child);
  int n = list.numberOfChildren();
  if (n == 0) {
    return Complex<T>::Undefined();
  }
  list = list.sort();

  /* Do not take undef children into account.
   * This loop relies on the fact that undef are sorted at the
   * end of the list. If ListSort::k_nanIsGreatest is changed, this
   * loop also needs to be changed. */
  int numberOfDefinedElements = 0;
  for (int i = n - 1 ; i >= 0 ; i--) {
    std::complex<T> c = list.complexAtIndex(i);
    if (!std::isnan(c.real()) || !std::isnan(c.imag())) {
      numberOfDefinedElements = i + 1;
      break;
    }
  }

  if (numberOfDefinedElements == 0) {
    return Complex<T>::Undefined();
  }

  if (numberOfDefinedElements % 2 == 1) {
    return Complex<T>::Builder(list.complexAtIndex(numberOfDefinedElements / 2));
  } else {
    return MultiplicationNode::Compute<T>(
      AdditionNode::computeOnComplex<T>(
        list.complexAtIndex(numberOfDefinedElements / 2 - 1),
        list.complexAtIndex(numberOfDefinedElements / 2),
        approximationContext.complexFormat()),
      Complex<T>::Builder(0.5),
      approximationContext.complexFormat());
  }
}

Expression ListMedian::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression sorted = ListSort::Builder(childAtIndex(0)).shallowReduce(reductionContext);
  int n = sorted.numberOfChildren();
  if (sorted.type() != ExpressionNode::Type::List || n == 0) {
    return replaceWithUndefinedInPlace();
  }

  /* Do not take undef children into account.
   * This loop relies on the fact that undef are sorted at the
   * end of the list. If ListSort::k_nanIsGreatest is changed, this
   * loop also needs to be changed. */
  int numberOfDefinedElements = 0;
  for (int i = n - 1 ; i >= 0 ; i--) {
    if (!sorted.childAtIndex(i).isUndefined()) {
      numberOfDefinedElements = i + 1;
      break;
    }
  }

  if (numberOfDefinedElements == 0) {
    return replaceWithUndefinedInPlace();
  }

  Expression e;
  if (numberOfDefinedElements % 2 == 1) {
    Expression e = sorted.childAtIndex(numberOfDefinedElements / 2);
    replaceWithInPlace(e);
    return e;
  } else {
    Expression a = sorted.childAtIndex(numberOfDefinedElements / 2 - 1), b = sorted.childAtIndex(numberOfDefinedElements / 2);
    Addition sum = Addition::Builder(a, b);
    Division div = Division::Builder(sum, Rational::Builder(2));
    sum.shallowReduce(reductionContext);
    replaceWithInPlace(div);
    return div.shallowReduce(reductionContext);
  }
}

template Evaluation<float> ListMedianNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListMedianNode::templatedApproximate<double>(ApproximationContext approximationContext) const;

}
