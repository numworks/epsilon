#include "infinity.h"

#include <poincare/src/memory/pattern_matching.h>

#include "approximation.h"
#include "k_tree.h"

namespace Poincare::Internal {

bool Infinity::IsPlusOrMinusInfinity(const Tree* u) {
  return u->isInf() || IsMinusInfinity(u);
}

bool Infinity::IsMinusInfinity(const Tree* u) {
  return u->treeIsIdenticalTo(KMult(-1_e, KInf));
}

bool Infinity::HasInfinityChild(Tree* u) {
  int n = u->numberOfChildren();
  Tree* child = u->firstChild();
  for (int i = 0; i < n; i++) {
    if (child->isInf()) {
      return true;
    }
    child = child->nextTree();
  }
  return false;
}

}  // namespace Poincare::Internal
