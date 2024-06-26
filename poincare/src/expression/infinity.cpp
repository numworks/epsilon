#include "infinity.h"

#include <poincare/src/memory/pattern_matching.h>

#include "approximation.h"
#include "k_tree.h"

namespace Poincare::Internal {

bool Infinity::IsPlusOrMinusInfinity(const Tree* e) {
  return e->isInf() || IsMinusInfinity(e);
}

bool Infinity::IsMinusInfinity(const Tree* e) {
  return e->treeIsIdenticalTo(KMult(-1_e, KInf));
}

}  // namespace Poincare::Internal
