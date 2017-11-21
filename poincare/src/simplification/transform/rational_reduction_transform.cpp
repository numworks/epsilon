#include "transform.h"
#include <assert.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/arithmetic.h>
#include <poincare/integer.h>

bool Poincare::Simplification::RationalReductionTransform(Expression * captures[]) {
  Multiplication * m = static_cast<Multiplication *>(captures[0]);
  Power * p = static_cast<Power *>(captures[2]);
  Integer * i1 = (Integer *)(captures[1]);
  Integer * i2 = (Integer *)(captures[3]);
  Integer gcd = Arithmetic::GCD(i1, i2);
  Integer one(1);
  if (gcd.compareTo(&one) == 0) {
    return false;
  }
  Integer * r1 = new Integer(Integer::Division(*i1, gcd).quotient);
  Integer * r2 = new Integer(Integer::Division(*i2, gcd).quotient);
  m->replaceOperand(i1, r1, true);
  p->replaceOperand(i2, r2, true);
  return true;
}
