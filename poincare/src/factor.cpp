#include <poincare/factor.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Factor::s_functionHelper;

int FactorNode::numberOfChildren() const { return Factor::s_functionHelper.numberOfChildren(); }

Layout FactorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Factor(this), floatDisplayMode, numberOfSignificantDigits, Factor::s_functionHelper.name());
}

int FactorNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Factor::s_functionHelper.name());
}

Expression FactorNode::shallowBeautify(ReductionContext reductionContext) {
  return Factor(this).shallowBeautify(reductionContext);
}

Expression Factor::shallowBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression c = childAtIndex(0);
  if (c.type() != ExpressionNode::Type::Rational) {
    Expression result = Undefined::Builder();
    replaceWithInPlace(result);
    return result;
  }
  Rational r = static_cast<Rational &>(c);
  if (r.isZero()) {
    replaceWithInPlace(r);
    return r;
  }
  Multiplication numeratorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.unsignedIntegerNumerator(), reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  if (numeratorDecomp.numberOfChildren() == 0) {
    Expression result = Undefined::Builder();
    replaceWithInPlace(result);
    return result;
  }
  Expression result = numeratorDecomp.squashUnaryHierarchyInPlace();
  if (!r.integerDenominator().isOne()) {
    Multiplication denominatorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.integerDenominator(), reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    if (denominatorDecomp.numberOfChildren() == 0) {
      Expression result = Undefined::Builder();
      replaceWithInPlace(result);
      return result;
    }
    result = Division::Builder(result, denominatorDecomp.squashUnaryHierarchyInPlace());
  }
  if (r.sign() == ExpressionNode::Sign::Negative) {
    result = Opposite::Builder(result);
  }
  replaceWithInPlace(result);
  return result;
}

Multiplication Factor::createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  //TODO LEA Matrices?
  assert(!i.isZero());
  assert(!i.isNegative());
  Multiplication m = Multiplication::Builder();
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  int numberOfPrimeFactors = Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (numberOfPrimeFactors == 0) {
    m.addChildAtIndexInPlace(Rational::Builder(i), 0, 0);
    return m;
  }
  if (numberOfPrimeFactors < 0) {
    // Exception: the decomposition failed
    return m;
  }
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    Expression factor = Rational::Builder(factors[index]);
    if (!coefficients[index].isOne()) {
      factor = Power::Builder(factor, Rational::Builder(coefficients[index]));
    }
    m.addChildAtIndexInPlace(factor, m.numberOfChildren(), m.numberOfChildren());
  }
  return m;
}

}
