#include <poincare/prediction_interval.h>
#include <poincare/matrix.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/undefined.h>
#include <poincare/division.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

PredictionIntervalNode * PredictionIntervalNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<PredictionIntervalNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutReference PredictionIntervalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(PredictionInterval(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression PredictionIntervalNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  return PredictionInterval(this).shallowReduce(context, angleUnit, futureParent);
}

template<typename T>
Evaluation<T> PredictionIntervalNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> pInput = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> nInput = childAtIndex(1)->approximate(T(), context, angleUnit);
  T p = static_cast<Complex<T> &>(pInput).toScalar();
  T n = static_cast<Complex<T> &>(nInput).toScalar();
  if (std::isnan(p) || std::isnan(n) || n != (int)n || n < 0 || p < 0 || p > 1) {
    return Complex<T>::Undefined();
  }
  std::complex<T> operands[2];
  operands[0] = std::complex<T>(p - 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n));
  operands[1] = std::complex<T>(p + 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n));
  return MatrixComplex<T>(operands, 1, 2);
}

Expression PredictionInterval::shallowReduce(Context& context, Preferences::AngleUnit angleUnit, const Expression futureParent) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression op0 = childAtIndex(0);
  Expression op1 = childAtIndex(1);
#if MATRIX_EXACT_REDUCING
  if (op0.type() == ExpressionNode::Type::Matrix || op1.type() == ExpressionNode::Type::Matrix) {
    return Undefined();
  }
#endif
  if (op0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(op0);
    if (r0.sign() == ExpressionNode::Sign::Negative || Integer::NaturalOrder(r0.unsignedIntegerNumerator(), r0.integerDenominator()) > 0) {
      return Undefined();
    }
  }
  if (op1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational &>(op1);
    if (!r1.integerDenominator().isOne() || r1.sign() == ExpressionNode::Sign::Negative) {
      return Undefined();
    }
  }
  if (op0.type() != ExpressionNode::Type::Rational || op1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational &>(op0);
  Rational r1 = static_cast<Rational &>(op1);
  if (!r1.integerDenominator().isOne() || r1.sign() == ExpressionNode::Sign::Negative || r0.sign() == ExpressionNode::Sign::Negative || Integer::NaturalOrder(r0.unsignedIntegerNumerator(), r0.integerDenominator()) > 0) {
    return Undefined();
  }
  /* [r0-1.96*sqrt(r0*(1-r0)/r1), r0+1.96*sqrt(r0*(1-r0)/r1)]*/
  // Compute numerator = r0*(1-r0)
  Rational numerator = Rational::Multiplication(r0, Rational(Integer::Subtraction(r0.integerDenominator(), r0.unsignedIntegerNumerator()), r0.integerDenominator()));
  if (numerator.numeratorOrDenominatorIsInfinity()) {
    return *this;
  }
  // Compute sqr = sqrt(r0*(1-r0)/r1)
  Expression sqr = Power(Division(numerator, r1), Rational(1, 2));
  Expression m = Multiplication(Rational(196, 100), sqr);
  Matrix matrix;
  matrix.addChildAtIndexInPlace(Addition(r0, Multiplication(Rational(-1), m)), 0, 0);
  matrix.addChildAtIndexInPlace(Addition(r0, m), 1, 1);
  matrix.setDimensions(1, 2);
  return matrix.deepReduce(context, angleUnit);
}

}
