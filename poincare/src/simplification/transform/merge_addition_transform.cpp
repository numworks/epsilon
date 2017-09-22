#include "merge_addition_transform.h"
#include <assert.h>
#include <poincare/addition.h>

namespace Poincare {
namespace Simplification {

void MergeAdditionTransform::apply(Expression * captures[]) const {
  assert(captures[0]->type() == Expression::Type::Addition);
  assert(captures[1]->type() == Expression::Type::Addition);

  Addition * a0 = (Addition *)(captures[0]);
  Addition * a1 = (Addition *)(captures[1]);
  a0->removeOperand(a1, false);
  a0->addOperands(a1->operands(), a1->numberOfOperands());
  a1->detachOperands();
  delete a1;
}

}
}
