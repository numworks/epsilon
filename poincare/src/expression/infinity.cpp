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

}  // namespace Poincare::Internal
