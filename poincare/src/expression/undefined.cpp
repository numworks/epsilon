#include "undefined.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

bool Undefined::CanBeUndefined(const Tree* e) {
  // Their elements can be undefined and it is never bubbled up.
  return !(e->isPoint() || e->isList() || e->isMatrix());
}

bool Undefined::CanHaveUndefinedChild(const Tree* e, int childIndex) {
  return !CanBeUndefined(e) || (e->isPiecewise() && childIndex % 2 == 0);
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
