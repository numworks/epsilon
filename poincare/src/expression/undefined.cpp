#include "undefined.h"

#include <poincare/src/memory/tree.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare::Internal {

Undefined::Type Undefined::GetType(const Tree* undefined) {
  return static_cast<Type>(undefined->nodeValue(0));
}

// Override Tree with Undefined tree.
void Undefined::Set(Tree* e, Type type) {
  e->moveTreeOverTree(SharedTreeStack->push<Internal::Type::Undef>(type));
}

Tree* Undefined::Push(Type type) {
  return SharedTreeStack->push<Internal::Type::Undef>(type);
}

bool Undefined::ShallowBubbleUpUndef(Tree* e) {
  if (e->isPoint() || e->isList()) {
    // Children can be undef
    return false;
  }
  uint8_t i = 0;
  bool hasUndefined = false;
  Type worstType = Type::None;
  for (const Tree* child : e->children()) {
    // Piecewise can have undefined branches
    if (!child->isUndef() || (e->isPiecewise() && i % 2 == 0)) {
      i++;
      continue;
    }
    Type childType = GetType(child);
    worstType = childType > worstType ? childType : worstType;
    hasUndefined = true;
    i++;
  }
  if (!hasUndefined) {
    return false;
  }
  Set(e, worstType);
  return true;
}

}  // namespace Poincare::Internal
