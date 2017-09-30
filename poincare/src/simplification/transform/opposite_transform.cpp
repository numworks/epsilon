#include "transform.h"
#include <assert.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>

bool Poincare::Simplification::OppositeTransform(Expression * captures[]) {
  Opposite * o = static_cast<Opposite *>(captures[0]);
  assert(o->type() == Expression::Type::Opposite);

  const Integer * minusOne = new Integer(-1);
  const Expression * multiplicationOperands[2] = {o->operand(0), minusOne};
  Multiplication * m = new Multiplication(multiplicationOperands, 2, false);

  o->detachOperands();
  o->replaceWith(m);

  return true;
}
