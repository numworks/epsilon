#ifndef POINCARE_MEMORY_TREE_HELPERS_H
#define POINCARE_MEMORY_TREE_HELPERS_H

#include "poincare/src/memory/tree_ref.h"
#include "poincare/src/memory/type_block.h"

namespace Poincare::Internal {

static constexpr Type MarkerBlock = Type::Zero;

/* A marker is a small block (a Zero block is used) referred to by a TreeRef.
 * It allows to mark and track a certain location in the TreeStack, without
 * modifying existing TreeRefs. Note that raw pointers to Trees located after
 * the marker will become invalid (which is expected). */
inline TreeRef pushMarker(Tree* location) {
  return location->cloneTreeBeforeNode(KTree<MarkerBlock>());
}

/* When removing a marker, this function asserts that the marker block was not
 * modified during its lifetime. */
inline void removeMarker(TreeRef& marker) {
  assert(marker->isOfType({MarkerBlock}));
  marker->removeTree();
}

}  // namespace Poincare::Internal

#endif
