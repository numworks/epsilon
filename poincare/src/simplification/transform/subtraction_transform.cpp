#include "subtraction_transform.h"
#include <assert.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <utility>

namespace Poincare {

void SubtractionTransform::apply(Expression * root, Expression * captures[]) const {
  assert(captures[0]->type() == Expression::Type::Subtraction);
  assert(captures[0] == root);

  Subtraction * s = (subtraction *)(root);

  Expression * operands[] = {
    new Integer(-1),
    s->operands(1)
  };

  Multiplication * m = new Multiplication(operands, 2, false);

  Addition * a = new Addition(
  s->parent()->replaceOperand(s,

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
}

}
