#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>

bool Poincare::Simplification::DivisionTransform(Expression * captures[]) {
  assert(captures[0]->type() == Expression::Type::Division);

  Division * d = static_cast<Division *>(captures[0]);
  assert(d->numberOfOperands() == 2);

  const Integer * minusOne = new Integer(-1);
  const Expression * powerOperands[2] = {d->operand(1), minusOne};
  const Power * p = new Power(powerOperands, false);
  const Expression * multiplicationOperands[2] = {d->operand(0), p};
  Multiplication * m = new Multiplication(multiplicationOperands, 2, false);

  static_cast<Hierarchy *>(d->parent())->replaceOperand(d, m, false);

  d->detachOperands();
  delete d;

  return true;
}
