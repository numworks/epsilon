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

Expression::Type Factor::type() const {
  return Type::Factor;
}

Expression * Factor::clone() const {
  Factor * b = new Factor(m_operands, true);
  return b;
}

Expression * Factor::shallowBeautify(Context& context, AngleUnit angleUnit) {
  Expression * op = editableOperand(0);
  if (op->type() != Type::Rational) {
    return new Undefined();
  }
  Rational * r = static_cast<Rational *>(op);
  if (r->isZero()) {
    return replaceWith(r, true);
  }
  Expression * numeratorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r->numerator(), context, angleUnit);
  Expression * result = numeratorDecomp;
  if (result->type() == Type::Undefined) {
    return replaceWith(result, true);
  }
  assert(numeratorDecomp->type() == Type::Multiplication);
  if (!r->denominator().isOne()) {
    Expression * denominatorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r->denominator(), context, angleUnit);
    if (denominatorDecomp->type() == Type::Undefined) {
      delete result;
      return replaceWith(denominatorDecomp, true);
    }
    assert(denominatorDecomp->type() == Type::Multiplication);
    result = new Division(numeratorDecomp, denominatorDecomp, false);
    static_cast<Multiplication *>(denominatorDecomp)->squashUnaryHierarchy();
  }
  if (r->sign() == Sign::Negative) {
    result = new Opposite(result, false);
  }
  replaceWith(result, true);
  if (result == numeratorDecomp) {
    return static_cast<Multiplication *>(numeratorDecomp)->squashUnaryHierarchy();
  }
  static_cast<Multiplication *>(numeratorDecomp)->squashUnaryHierarchy();
  return result;
}

Expression * Factor::createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context & context, AngleUnit angleUnit) {
  assert(!i.isZero());
  i.setNegative(false);
  Multiplication * m = new Multiplication();
  if (i.isOne()) {
    m->addOperand(new Rational(i));
    return m;
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  int index = 0;
  if (coefficients[0].isMinusOne()) {
    delete m;
    return new Undefined();
  }
  while (!coefficients[index].isZero() && index < Arithmetic::k_maxNumberOfPrimeFactors) {
    Expression * factor = new Rational(factors[index]);
    if (!coefficients[index].isOne()) {
      Expression * exponent = new Rational(coefficients[index]);
      factor = new Power(factor, exponent, false);
    }
    m->addOperand(factor);
    index++;
  }
  return m;
}

}
