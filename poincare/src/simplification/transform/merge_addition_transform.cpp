#include "merge_addition_transform.h"
#include <assert.h>
#include <poincare/addition.h>

namespace Poincare {
namespace Simplification {

void MergeAdditionTransform::apply(Expression * root, Expression * captures[]) const {
  assert(captures[0]->type() == Expression::Type::Addition);
  assert(captures[1]->type() == Expression::Type::Addition);
  assert(captures[1] == root);

  Addition * a0 = (Addition *)(captures[0]);
  Addition * a1 = (Addition *)(captures[1]);
  a1->removeOperand(a0, false);
  a1->addOperands(a0->operands(), a0->numberOfOperands());
  a0->detachOperands();
  delete a0;
}

}
}
