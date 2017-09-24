#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>

bool Poincare::Simplification::SubtractionTransform(Expression * captures[]) {
  assert(captures[0]->type() == Expression::Type::Subtraction);

  Subtraction * s = static_cast<Subtraction *>(captures[0]);
  assert(s->numberOfOperands() == 2);

  const Integer * minusOne = new Integer(-1);
  const Expression * multiplicationOperands[2] = {s->operand(1), minusOne};
  const Multiplication * m = new Multiplication(multiplicationOperands, 2, false);
  const Expression * additionOperands[2] = {s->operand(0), m};
  Addition * a = new Addition(additionOperands, 2, false);

  static_cast<Hierarchy *>(s->parent())->replaceOperand(s, a, false);

  s->detachOperands();
  delete s;

  return true;
}
