#ifndef POINCARE_MEMORY_TREE_HELPERS_H
#define POINCARE_MEMORY_TREE_HELPERS_H

#include "poincare/src/memory/tree_ref.h"
#include "poincare/src/memory/type_block.h"

namespace Poincare::Internal {

static constexpr AnyType MarkerBlock = Type::Zero;

/* An end marker is a small block (a Zero block is used) referred to by a
 * TreeRef. It allows to mark and track the end of a Tree, without modifying
 * existing TreeRefs. Note that raw pointers to Trees located after the marker
 * will become invalid (which is expected). */
inline TreeRef pushEndMarker(Tree* tree) {
  return tree->end()->cloneTreeBeforeNode(KTree<MarkerBlock>());
}

/* When removing a marker, this function asserts that the marker block was not
 * modified during its lifetime. */
inline void removeMarker(TreeRef& marker) {
  assert(marker->isOfType({MarkerBlock}));
  marker->removeTree();
}

}  // namespace Poincare::Internal

#endif
