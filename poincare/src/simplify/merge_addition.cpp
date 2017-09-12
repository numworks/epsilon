#include "merge_addition.h"
#include <assert.h>
#include <poincare/addition.h>

namespace Poincare {

void MergeAddition::apply(Expression * root, Expression * captures[]) const {
  assert(captures[0]->type() == Expression::Type::Addition);
  assert(captures[1]->type() == Expression::Type::Addition);
  assert(captures[1] == root);

  Addition * a0 = (Addition *)(captures[0]);
  Addition * a1 = (Addition *)(captures[1]);
  a1->stealOperandsFrom(a0);
  a1->removeOperand(a0);
}

}
