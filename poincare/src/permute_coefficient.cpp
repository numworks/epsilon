#include <poincare/permute_coefficient.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

PermuteCoefficientNode * PermuteCoefficientNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<PermuteCoefficientNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutReference PermuteCoefficientNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(PermuteCoefficient(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression PermuteCoefficientNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return PermuteCoefficient(this).shallowReduce(context, angleUnit);
}

template<typename T>
Evaluation<T> PermuteCoefficientNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> nInput = childAtIndex(0)->approximate(T(), context, angleUnit);
  Evaluation<T> kInput = childAtIndex(1)->approximate(T(), context, angleUnit);
  T n = nInput.toScalar();
  T k = kInput.toScalar();
  if (std::isnan(n) || std::isnan(k) || n != std::round(n) || k != std::round(k) || n < 0.0f || k < 0.0f) {
    return Complex<T>::Undefined();
  }
  if (k > n) {
    return Complex<T>(0.0);
  }
  T result = 1;
  for (int i = (int)n-(int)k+1; i <= (int)n; i++) {
    result *= i;
    if (std::isinf(result) || std::isnan(result)) {
      return Complex<T>(result);
    }
  }
  return Complex<T>(std::round(result));
}

Expression PermuteCoefficient::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression op0 = childAtIndex(0);
  Expression op1 = childAtIndex(1);
#if MATRIX_EXACT_REDUCING
  if (op0.type() == ExpressionNode::Type::Matrix || op1.type() == ExpressionNode::Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
#endif
  if (op0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(op0);
    if (!r0.integerDenominator().isOne() || r0.sign() == ExpressionNode::Sign::Negative) {
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

  Integer n = r0.unsignedIntegerNumerator();
  Integer k = r1.unsignedIntegerNumerator();
  if (n.isLowerThan(k)) {
    return Rational(0);
  }
  /* if n is too big, we do not reduce to avoid too long computation.
   * The permute coefficient will be evaluate approximatively later */
  if (Integer(k_maxNValue).isLowerThan(n)) {
    return *this;
  }
  Integer result(1);
  int clippedK = k.extractedInt(); // Authorized because k < n < k_maxNValue
  for (int i = 0; i < clippedK; i++) {
    Integer factor = Integer::Subtraction(n, Integer(i));
    result = Integer::Multiplication(result, factor);
  }
  return Rational(result);
}

}

