#include "transform.h"
#include <assert.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>

bool Poincare::Simplification::OppositeTransform(Expression * captures[]) {
  assert(captures[0]->type() == Expression::Type::Opposite);

  Opposite * o = static_cast<Opposite *>(captures[0]);

  const Integer * minusOne = new Integer(-1);
  const Expression * multiplicationOperands[2] = {o->operand(0), minusOne};
  Multiplication * m = new Multiplication(multiplicationOperands, 2, false);

  static_cast<Hierarchy *>(o->parent())->replaceOperand(o, m, false);

  o->detachOperands();
  delete o;

  return true;
}
