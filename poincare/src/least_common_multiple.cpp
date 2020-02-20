#include <poincare/least_common_multiple.h>
#include <poincare/approximation_helper.h>
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
  return LeastCommonMultiple(this).shallowReduce(reductionContext.context());
}

template<typename T>
Evaluation<T> LeastCommonMultipleNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  bool isUndefined = false;
  int a = ApproximationHelper::PositiveIntegerApproximationIfPossible<T>(childAtIndex(0), &isUndefined, context, complexFormat, angleUnit);
  int b = ApproximationHelper::PositiveIntegerApproximationIfPossible<T>(childAtIndex(1), &isUndefined, context, complexFormat, angleUnit);
  if (isUndefined) {
    return Complex<T>::RealUndefined();
  }
  if (a == 0 || b == 0) {
    return Complex<T>::Builder(0.0);
  }
  if (b > a) {
    int temp = b;
    b = a;
    a = temp;
  }
  int product = a*b;
  int r = 0;
  while((int)b!=0){
    r = a - (a/b)*b;
    a = b;
    b = r;
  }
  return Complex<T>::Builder(product/a);
}


Expression LeastCommonMultiple::shallowReduce(Context * context) {
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
    Rational r1 = static_cast<Rational &>(c1);
    if (!r1.isInteger()) {
      return replaceWithUndefinedInPlace();
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
