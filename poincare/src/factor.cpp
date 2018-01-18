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
  Multiplication * numeratorDecomp = addFactorsOfIntegerDecomposition(r->numerator(), context, angleUnit);
  Expression * result = numeratorDecomp;
  if (!r->denominator().isOne()) {
    Multiplication * denominatorDecomp = addFactorsOfIntegerDecomposition(r->denominator(), context, angleUnit);
    result = new Division(numeratorDecomp, denominatorDecomp, false);
    denominatorDecomp->squashUnaryHierarchy();
  }
  if (r->sign() == Sign::Negative) {
    result = new Opposite(result, false);
  }
  replaceWith(result, true);
  if (result == numeratorDecomp) {
    return numeratorDecomp->squashUnaryHierarchy();
  }
  numeratorDecomp->squashUnaryHierarchy();
  return result;
}

Multiplication * Factor::addFactorsOfIntegerDecomposition(Integer i, Context & context, AngleUnit angleUnit) {
  assert(!i.isZero());
  i.setNegative(false);
  Multiplication * m = new Multiplication();
  if (Arithmetic::k_primorial32.isLowerThan(i) || i.isOne()) {
    /* We do not want to break i in prime factor because it might be take too
     * many factors... More than k_maxNumberOfPrimeFactors. */
    m->addOperand(new Rational(i));
    return m;
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(&i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  int index = 0;
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
