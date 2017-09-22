#include "integer_addition.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/integer.h>
#include <utility>

namespace Poincare {

void IntegerAddition::apply(Expression * root, Expression * captures[]) const {
  assert(captures[0]->type() == Expression::Type::Integer);
  assert(captures[1]->type() == Expression::Type::Integer);
  assert(captures[2]->type() == Expression::Type::Addition);
  assert(captures[2] == root);
#if 0

  Integer * i1 = (Integer *)(captures[0]);
  Integer * i2 = (Integer *)(captures[1]);
  Addition * a = (Addition *)(captures[2]);

  Integer resultOnStack = i1->add(*i2);
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
#endif
}

}
