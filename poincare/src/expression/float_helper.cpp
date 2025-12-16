#include "float_helper.h"

#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

bool FloatHelper::SetSign(Tree* e, NonStrictSign sign) {
  double value = To(e);
  if (value == 0 || (value > 0.0) == (sign == NonStrictSign::Positive)) {
    return false;
  }
  e->moveTreeOverTree(
      e->isSingleFloat()
          ? SharedTreeStack->pushSingleFloat(-static_cast<float>(value))
          : SharedTreeStack->pushDoubleFloat(-value));
  return true;
}

void FloatHelper::ToOppositeFloat(Tree* e) {
  assert(e->isFloat());
  assert(To(e) < 0);
  Tree* opposite = SharedTreeStack->pushOpposite();
  Tree* valueTree = e->cloneTree();
  [[maybe_unused]] bool positiveSignWasSet =
      SetSign(valueTree, NonStrictSign::Positive);
  assert(positiveSignWasSet);
  e->moveTreeOverTree(opposite);
}

}  // namespace Poincare::Internal
