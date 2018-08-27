#include <poincare/least_common_multiple.h>
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/layout_helper.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

LeastCommonMultipleNode * LeastCommonMultipleNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<LeastCommonMultipleNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutReference LeastCommonMultipleNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(LeastCommonMultiple(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression LeastCommonMultipleNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return LeastCommonMultiple(this).shallowReduce(context, angleUnit);
}

template<typename T>
Evaluation<T> LeastCommonMultipleNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> f1Input = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> f2Input = childAtIndex(1)->approximate(T(), context, angleUnit);
  T f1 = f1Input.toScalar();
  T f2 = f2Input.toScalar();
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return Complex<T>::Undefined();
  }
  if (f1 == 0.0f || f2 == 0.0f) {
    return Complex<T>(0.0);
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
  return Complex<T>(product/a);
}

Expression LeastCommonMultiple::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression op0 = childAtIndex(0);
  Expression op1 = childAtIndex(1);
#if MATRIX_EXACT_REDUCING
  if (op0.type() == Type::Matrix || op1.type() == Type::Matrix) {
    return Undefined();
  }
#endif
  if (op0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational&>(op0);
    if (!r0.integerDenominator().isOne()) {
      return Undefined();
    }
  }
  if (op1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational&>(op1);
    if (!r1.integerDenominator().isOne()) {
      return Undefined();
    }
  }
  if (op0.type() != ExpressionNode::Type::Rational || op1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational&>(op0);
  Rational r1 = static_cast<Rational&>(op1);

  Integer a = r0.signedIntegerNumerator();
  Integer b = r1.signedIntegerNumerator();
  Integer lcm = Arithmetic::LCM(a, b);
  return Rational(lcm);
}

}

