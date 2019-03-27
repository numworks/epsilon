#include <poincare/great_common_divisor.h>

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

Expression GreatCommonDivisorNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return GreatCommonDivisor(this).shallowReduce();
}

template<typename T>
Evaluation<T> GreatCommonDivisorNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> f1Input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> f2Input = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  T f1 = f1Input.toScalar();
  T f2 = f2Input.toScalar();
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return Complex<T>::Undefined();
  }
  int a = (int)f2;
  int b = (int)f1;
  if (f1 > f2) {
    b = a;
    a = (int)f1;
  }
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return Complex<T>::Builder(std::round((T)a));
}


Expression GreatCommonDivisor::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
#if MATRIX_EXACT_REDUCING
  if (c0.type() == ExpressionNode::Type::Matrix || c1.type() == ExpressionNode::Type::Matrix) {
    return Undefined::Builder();
  }
#endif
  if (c0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (!r0.integerDenominator().isOne()) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
    }
  }
  if (c1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational&>(c1);
    if (!r1.integerDenominator().isOne()) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
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
