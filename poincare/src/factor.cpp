#include <poincare/factor.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>

extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>

namespace Poincare {

FactorNode * FactorNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<FactorNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

LayoutRef FactorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Factor(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression FactorNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Factor(this).shallowBeautify(context, angleUnit);
}

Expression Factor::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  Expression op = childAtIndex(0);
  if (op.type() != ExpressionNode::Type::Rational) {
    return Undefined();
  }
  Rational r = static_cast<Rational &>(op);
  if (r.isZero()) {
    return r;
  }
  Multiplication numeratorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.unsignedIntegerNumerator(), context, angleUnit);
  if (numeratorDecomp.numberOfChildren() == 0) {
    return Undefined();
  }
  Expression result = numeratorDecomp.squashUnaryHierarchy();
  if (!r.integerDenominator().isOne()) {
    Multiplication denominatorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.integerDenominator(), context, angleUnit);
    if (denominatorDecomp.numberOfChildren() == 0) {
      return Undefined();
    }
    result = Division(numeratorDecomp, denominatorDecomp.squashUnaryHierarchy());
  }
  if (r.sign() == ExpressionNode::Sign::Negative) {
    result = Opposite(result);
  }
  return result;
}

Multiplication Factor::createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(!i.isZero());
  assert(!i.isNegative());
  Multiplication m;
  if (i.isOne()) {
    m.addChildAtIndexInPlace(Rational(i), 0, 0);
    return m;
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  int index = 0;
  if (coefficients[0].isMinusOne()) {
    // Exception: the decomposition failed
    return m;
  }
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    Expression factor = Rational(factors[index]);
    if (!coefficients[index].isOne()) {
      factor = Power(factor, Rational(coefficients[index]));
    }
    m.addChildAtIndexInPlace(factor, m.numberOfChildren(), m.numberOfChildren());
    index++;
  }
  return m;
}

}
