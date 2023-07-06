#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <string.h>

#include "edition_pool.h"
#include "node_iterator.h"
#include "pattern_matching.h"

namespace PoincareJ {

EditionReference::EditionReference(const Node* node) {
  if (!node) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  // TODO: maybe make an assertion(pool->contains(node->block()))
  // and force developers to write EditionReference(EditionPool::clone(2_e))
  if (!editionPool->contains(node->block()) &&
      node->block() != editionPool->lastBlock()) {
    *this = EditionReference(editionPool->clone(node));
    return;
  }
  m_identifier = editionPool->referenceNode(const_cast<Node*>(node));
}

EditionReference::EditionReference(Node* node) {
  if (!node) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  assert(editionPool->contains(node->block()) ||
         node->block() == editionPool->lastBlock());
  m_identifier = editionPool->referenceNode(node);
}

#if POINCARE_MEMORY_TREE_LOG
void EditionReference::log() const {
  std::cout << "id: " << m_identifier << "\n";
  node()->log(std::cout, true, 1, true);
}
#endif

Node* EditionReference::node() const {
  return editionPool->nodeForIdentifier(m_identifier);
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

}  // namespace PoincareJ
