#include "transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/integer.h>
#include <utility>

bool Poincare::Simplification::IntegerAdditionTransform(Expression * captures[]) {
  assert(captures[0]->type() == Expression::Type::Addition);
  assert(captures[1]->type() == Expression::Type::Integer);
  assert(captures[2]->type() == Expression::Type::Integer);

  Addition * a = (Addition *)(captures[0]);
  Integer * i1 = (Integer *)(captures[1]);
  Integer * i2 = (Integer *)(captures[2]);

  Integer resultOnStack = Integer::Addition(*i1, *i2);
  Integer * r = new Integer(std::move(resultOnStack));
  //r->add(resultOnStack);
  // FIXME: Beeeeuargl

  if (a->numberOfOperands() == 2) {
    ((Hierarchy *)a->parent())->replaceOperand(a, r);
  } else {
    assert(a->numberOfOperands() > 2);
    a->replaceOperand(i1, r);
    a->removeOperand(i2);
  }

  return true;
}
