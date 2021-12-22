#include <poincare/list_maximum.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>

namespace Poincare {

const Expression::FunctionHelper ListMaximum::s_functionHelper;

int ListMaximumNode::numberOfChildren() const {
  return ListMaximum::s_functionHelper.numberOfChildren();
}

int ListMaximumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ListMaximum::s_functionHelper.name());
}

Layout ListMaximumNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ListMaximum(this), floatDisplayMode, numberOfSignificantDigits, ListMaximum::s_functionHelper.name());
}

Expression ListMaximumNode::shallowReduce(ReductionContext reductionContext) {
  return ListMaximum(this).shallowReduce(reductionContext);
}

template<typename T> Evaluation<T> ListMaximumNode::templatedApproximate(ApproximationContext approximationContext) const {
  ExpressionNode * child = childAtIndex(0);
  if (child->type() != ExpressionNode::Type::List) {
    return Complex<T>::Undefined();
  }

  Evaluation<T> result = Complex<T>::Undefined();
  float value = -INFINITY;
  int n = child->numberOfChildren();
  for (int i = 0; i < n; i++) {
    Evaluation<T> candidate = child->childAtIndex(i)->approximate(static_cast<T>(0), approximationContext);
    float newValue = candidate.toScalar();
    if (std::isnan(newValue)) {
      return Complex<T>::Undefined();
    }
    if (newValue > value) {
      result = candidate;
      value = newValue;
    }
  }
  return result;
}

Expression ListMaximum::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression child = childAtIndex(0);
  if (child.type() != ExpressionNode::Type::List) {
    return replaceWithUndefinedInPlace();
  }

  Context * context = reductionContext.context();
  Preferences::ComplexFormat complexFormat = reductionContext.complexFormat();
  Preferences::AngleUnit angleUnit = reductionContext.angleUnit();

  Expression result = Undefined::Builder();
  float value = -INFINITY;
  int n = child.numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression candidate = child.childAtIndex(i);
    float newValue = candidate.approximateToScalar<float>(context, complexFormat, angleUnit, true);
    if (std::isnan(newValue)) {
      return replaceWithUndefinedInPlace();
    }
    if (newValue > value) {
      result = candidate;
      value = newValue;
    }
  }
  replaceWithInPlace(result);
  return result;
}

}
