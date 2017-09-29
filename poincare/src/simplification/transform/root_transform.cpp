#include "transform.h"
#include <assert.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>

namespace Poincare {

static bool privateRootTransform(Expression * captures[], const Expression * index) {
  Hierarchy * h = static_cast<Hierarchy *>(captures[0]);
  const Integer * minusOne = new Integer(-1);
  const Expression * power0Operands[2] = {index, minusOne};
  const Power * p0 = new Power(power0Operands, false);
  const Expression * power1Operands[2] = {h->operand(0), p0};
  Power * p1 = new Power(power1Operands, false);
  static_cast<Hierarchy *>(h->parent())->replaceOperand(h, p1, false);
  h->detachOperands();
  delete h;
  return true;
}

bool Simplification::SquareRootTransform(Expression * captures[]) {
  const Integer * two = new Integer(2);
  return privateRootTransform(captures, two);
}

bool Simplification::NthRootTransform(Expression * captures[]) {
  Hierarchy * h = static_cast<Hierarchy *>(captures[0]);
  return privateRootTransform(captures, h->operand(1));
}

}
