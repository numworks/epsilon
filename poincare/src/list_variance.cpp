#include <poincare/list_variance.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/list_mean.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>

namespace Poincare {

const Expression::FunctionHelper ListVariance::s_functionHelper;

int ListVarianceNode::numberOfChildren() const {
  return ListVariance::s_functionHelper.numberOfChildren();
}

int ListVarianceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListVariance::s_functionHelper.name());
}

Layout ListVarianceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListVariance(this), floatDisplayMode, numberOfSignificantDigits, ListVariance::s_functionHelper.name());
}

Expression ListVarianceNode::shallowReduce(ReductionContext reductionContext) {
  return ListVariance(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListVarianceNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List || child->numberOfChildren() == 0) {
    return Complex<T>::Undefined();
  }

  return static_cast<ListNode *>(child)->variance<T>(approximationContext);
}

Expression ListVariance::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  /* var(L) = mean(L^2) - mean(L)^2 */
  Expression child = childAtIndex(0);
  ListMean m = ListMean::Builder(child.clone());
  Power m2 = Power::Builder(m, Rational::Builder(2));
  m.shallowReduce(reductionContext);
  Power l2 = Power::Builder(child, Rational::Builder(2));
  ListMean ml2 = ListMean::Builder(l2);
  l2.shallowReduce(reductionContext);
  Subtraction s = Subtraction::Builder(ml2, m2);
  ml2.shallowReduce(reductionContext);
  m2.shallowReduce(reductionContext);
  replaceWithInPlace(s);
  return s.shallowReduce(reductionContext);
}

template Evaluation<float> ListVarianceNode::templatedApproximate<float>(ApproximationContext approximationContext) const;
template Evaluation<double> ListVarianceNode::templatedApproximate<double>(ApproximationContext approximationContext) const;


}
