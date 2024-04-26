#include "undefined.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

bool Undefined::ShallowBubbleUpUndef(Tree* e) {
  if (e->isPoint() || e->isList()) {
    // Children can be undef
    return false;
  }
  uint8_t i = 0;
  Type worstType = Type::Zero;
  for (const Tree* child : e->children()) {
    // Piecewise can have undefined branches, but not conditions
    if (child->isUndefined() && !(e->isPiecewise() && i % 2 == 0)) {
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
