#include "undefined.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

bool Undefined::CanHaveUndefinedChild(Tree* e, int childIndex) {
  return e->isPoint() || e->isList() ||
         (e->isPiecewise() && childIndex % 2 == 0);
}

bool Undefined::ShallowBubbleUpUndef(Tree* e) {
  uint8_t i = 0;
  Type worstType = Type::Zero;
  for (const Tree* child : e->children()) {
    // Piecewise can have undefined branches, but not conditions
    if (child->isUndefined() && !CanHaveUndefinedChild(e, i)) {
      Type childType = child->type();
      worstType = childType > worstType ? childType : worstType;
    }
    i++;
  }
  if (worstType == Type::Zero) {
    return false;
  }
  e->moveTreeOverTree(SharedTreeStack->push(worstType));
  assert(e->isUndefined());
  return true;
}

}  // namespace Poincare::Internal
