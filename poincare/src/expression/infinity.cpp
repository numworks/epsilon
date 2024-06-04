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

bool Infinity::HasInfinityChild(const Tree* u) {
  for (const Tree* child : u->children()) {
    if (child->isInf()) {
      return true;
    }
  }
  return false;
}

}  // namespace Poincare::Internal
