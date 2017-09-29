#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/arithmetic.h>

bool Poincare::Simplification::RationalPowerTransform(Expression * captures[]) {
  Power * p0 = static_cast<Power *>(captures[0]);
  Integer * i0 = static_cast<Integer *>(captures[1]);
  Multiplication * m = static_cast<Multiplication *>(captures[2]);
  Integer * i1 = static_cast<Integer *>(captures[3]);
  Power * p1 = static_cast<Power *>(captures[4]);
  Integer * i2 = static_cast<Integer *>(captures[5]);

  if (i0->isEqualTo(Integer(1)) || i0->isEqualTo(Integer(0)) || Arithmetic::k_primorial32.isLowerThan(*i0)) {
    // We do not want to factorize number above
    return false;
  }
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  Arithmetic::PrimeFactorization(i0, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);

  Integer r1 = Integer(1);
  Integer r2 = Integer(1);
  int index = 0;
  while (Integer(0).isLowerThan(coefficients[index])) {
    Integer n = Integer::Multiplication(coefficients[index], *i1);
    IntegerDivision div = Integer::Division(n, *i2);
    if (div.remainder.isEqualTo(Integer(0))) {
      r1 = Integer::Multiplication(r1, Integer::Power(factors[index], div.quotient));
    } else {
      r2 = Integer::Multiplication(r2, Integer::Power(factors[index], n));
    }
    index++;
  }
  if (r1.isEqualTo(Integer(1)) && !i0->isNegative()) {
    return false;
  }
  if (i0->isNegative()) {
  // TODO if i0.isNegtative?
  }
  m->replaceOperand(i1, new Integer(r1), true);
  const Expression * powerOperands[2] = {new Integer(r2), p1};
  Power * p2 = new Power(powerOperands, false);
  m->replaceOperand(p1, p2, false);

  p0->replaceOperand(m, nullptr, false);
  static_cast<Hierarchy *>(p0->parent())->replaceOperand(p0, m, true);
  return true;
}
