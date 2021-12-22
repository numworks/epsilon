#include <poincare/list_minimum.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

const Expression::FunctionHelper ListMinimum::s_functionHelper;

int ListMinimumNode::numberOfChildren() const {
  return ListMinimum::s_functionHelper.numberOfChildren();
}

int ListMinimumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMinimum::s_functionHelper.name());
}

Layout ListMinimumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListMinimum(this), floatDisplayMode, numberOfSignificantDigits, ListMinimum::s_functionHelper.name());
}

Expression ListMinimumNode::shallowReduce(ReductionContext reductionContext) {
  return ListMinimum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMinimumNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  Evaluation<T> result = Complex<T>::Undefined();
  float value = INFINITY;
  int n = child->numberOfChildren();
  for (int i = 0; i < n; i++) {
    Evaluation<T> candidate = child->childAtIndex(i)->approximate(static_cast<T>(0), approximationContext);
    float newValue = candidate.toScalar();
    if (std::isnan(newValue)) {
      return Complex<T>::Undefined();
    }
    if (newValue < value) {
      result = candidate;
      value = newValue;
    }
  }
  return result;
}

Expression ListMinimum::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  Context * context = reductionContext.context();
  Preferences::ComplexFormat complexFormat = reductionContext.complexFormat();
  Preferences::AngleUnit angleUnit = reductionContext.angleUnit();

  Expression result = Undefined::Builder();
  float value = INFINITY;
  int n = child.numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression candidate = child.childAtIndex(i);
    float newValue = candidate.approximateToScalar<float>(context, complexFormat, angleUnit, true);
    if (std::isnan(newValue)) {
      return replaceWithUndefinedInPlace();
    }
    if (newValue < value) {
      result = candidate;
      value = newValue;
    }
  }
  replaceWithInPlace(result);
  return result;
}

}
