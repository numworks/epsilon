#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <string.h>

#include "edition_pool.h"
#include "node_iterator.h"
#include "pattern_matching.h"

namespace PoincareJ {

EditionReference::EditionReference(const Tree* node) {
  if (!node) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  // TODO: maybe make an assertion(pool->contains(node->block()))
  // and force developers to write EditionReference(EditionPool::clone(2_e))
  if (!SharedEditionPool->contains(node->block()) &&
      node->block() != SharedEditionPool->lastBlock()) {
    *this = EditionReference(SharedEditionPool->clone(node));
    return;
  }
  m_identifier = SharedEditionPool->referenceNode(const_cast<Tree*>(node));
}

EditionReference::EditionReference(Tree* node) {
  if (!node) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  assert(SharedEditionPool->contains(node->block()) ||
         node->block() == SharedEditionPool->lastBlock());
  m_identifier = SharedEditionPool->referenceNode(node);
}

#if POINCARE_MEMORY_TREE_LOG
void EditionReference::log() const {
  std::cout << "id: " << m_identifier << "\n";
  node()->log(std::cout, true, 1, true);
}
#endif

Tree* EditionReference::node() const {
  return SharedEditionPool->nodeForIdentifier(m_identifier);
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

}  // namespace PoincareJ
