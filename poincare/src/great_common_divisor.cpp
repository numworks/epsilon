#include <poincare/great_common_divisor.h>

#include <poincare/approximation_helper.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper GreatCommonDivisor::s_functionHelper;

int GreatCommonDivisorNode::numberOfChildren() const { return GreatCommonDivisor::s_functionHelper.numberOfChildren(); }

Layout GreatCommonDivisorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(GreatCommonDivisor(this), floatDisplayMode, numberOfSignificantDigits, GreatCommonDivisor::s_functionHelper.name());
}

int GreatCommonDivisorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, GreatCommonDivisor::s_functionHelper.name());
}

Expression GreatCommonDivisorNode::shallowReduce(ReductionContext reductionContext) {
  return GreatCommonDivisor(this).shallowReduce(reductionContext.context());
}

template<typename T>
Evaluation<T> GreatCommonDivisorNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  bool isUndefined = false;
  int a = ApproximationHelper::PositiveIntegerApproximationIfPossible<T>(childAtIndex(0), &isUndefined, context, complexFormat, angleUnit);
  int b = ApproximationHelper::PositiveIntegerApproximationIfPossible<T>(childAtIndex(1), &isUndefined, context, complexFormat, angleUnit);
  if (isUndefined) {
    return Complex<T>::RealUndefined();
  }
  if (b > a) {
    int temp = b;
    b = a;
    a = temp;
  }
  int r = 0;
  while((int)b!=0){
    r = a - (a/b)*b;
    a = b;
    b = r;
  }
  return Complex<T>::Builder(std::round((T)a));
}


Expression GreatCommonDivisor::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.deepIsMatrix(context) || c1.deepIsMatrix(context)) {
    return replaceWithUndefinedInPlace();
  }
  if (c0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (!r0.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational&>(c1);
    if (!r1.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational&>(c0);
  Rational r1 = static_cast<Rational&>(c1);

  Integer a = r0.signedIntegerNumerator();
  Integer b = r1.signedIntegerNumerator();
  Integer gcd = Arithmetic::GCD(a, b);
  assert(!gcd.isOverflow());
  Expression result = Rational::Builder(gcd);
  replaceWithInPlace(result);
  return result;
}

}
