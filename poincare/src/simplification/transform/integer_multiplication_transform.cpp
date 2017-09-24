#include "transform.h"
#include <assert.h>
#include <poincare/integer.h>
#include <poincare/multiplication.h>
#include <utility>

bool Poincare::Simplification::IntegerMultiplicationTransform(Expression * captures[]) {
  assert(captures[0]->type() == Expression::Type::Multiplication);
  assert(captures[1]->type() == Expression::Type::Integer);
  assert(captures[2]->type() == Expression::Type::Integer);

  Multiplication * a = static_cast<Multiplication *>(captures[0]);
  Integer * i1 = static_cast<Integer *>(captures[1]);
  Integer * i2 = static_cast<Integer *>(captures[2]);

  Integer resultOnStack = i1->multiply_by(*i2);
  Integer * r = new Integer(std::move(resultOnStack));
  //r->add(resultOnStack);
  // FIXME: Beeeeuargl

  if (a->numberOfOperands() == 2) {
    static_cast<Hierarchy *>(a->parent())->replaceOperand(a, r);
  } else {
    assert(a->numberOfOperands() > 2);
    a->replaceOperand(i1, r);
    a->removeOperand(i2);
  }

  return true;
}
