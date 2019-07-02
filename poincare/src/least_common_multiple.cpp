#include <poincare/least_common_multiple.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

constexpr Expression::FunctionHelper LeastCommonMultiple::s_functionHelper;

int LeastCommonMultipleNode::numberOfChildren() const { return LeastCommonMultiple::s_functionHelper.numberOfChildren(); }

Layout LeastCommonMultipleNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(LeastCommonMultiple(this), floatDisplayMode, numberOfSignificantDigits, LeastCommonMultiple::s_functionHelper.name());
}

int LeastCommonMultipleNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, LeastCommonMultiple::s_functionHelper.name());
}

Expression LeastCommonMultipleNode::shallowReduce(ReductionContext reductionContext) {
  return LeastCommonMultiple(this).shallowReduce();
}

template<typename T>
Evaluation<T> LeastCommonMultipleNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> f1Input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> f2Input = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  T f1 = f1Input.toScalar();
  T f2 = f2Input.toScalar();
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return Complex<T>::Undefined();
  }
  if (f1 == 0.0f || f2 == 0.0f) {
    return Complex<T>::Builder(0.0);
  }
  int a = (int)f2;
  int b = (int)f1;
  if (f1 > f2) {
    b = a;
    a = (int)f1;
  }
  int product = a*b;
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return Complex<T>::Builder(product/a);
}


Expression LeastCommonMultiple::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.type() == ExpressionNode::Type::Matrix || c1.type() == ExpressionNode::Type::Matrix) {
    Expression result = Undefined::Builder();
    replaceWithInPlace(result);
    return result;
  }
  if (c0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (!r0.integerDenominator().isOne()) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
    }
  }
  if (c1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational &>(c1);
    if (!r1.integerDenominator().isOne()) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
    }
  }
  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational &>(c0);
  Rational r1 = static_cast<Rational &>(c1);

  Integer a = r0.signedIntegerNumerator();
  Integer b = r1.signedIntegerNumerator();
  Integer lcm = Arithmetic::LCM(a, b);
  if (lcm.isOverflow()) {
    return *this;
  }
  Expression result = Rational::Builder(lcm);
  replaceWithInPlace(result);
  return result;
}

}
