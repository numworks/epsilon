#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_creator.h>
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
  EditionPool* pool = EditionPool::sharedEditionPool();
  // TODO: maybe make an assertion(pool->contains(node->block()))
  // and force developers to write EditionReference(EditionPool::clone(2_e))
  if (!pool->contains(node->block()) && node->block() != pool->lastBlock()) {
    *this = EditionReference(pool->clone(node));
    return;
  }
  m_identifier =
      EditionPool::sharedEditionPool()->referenceNode(const_cast<Node*>(node));
}

EditionReference::EditionReference(Node* node) {
  assert(EditionPool::sharedEditionPool()->contains(node->block()) ||
         node->block() == EditionPool::sharedEditionPool()->lastBlock());
  m_identifier = EditionPool::sharedEditionPool()->referenceNode(node);
}

#if POINCARE_MEMORY_TREE_LOG
void EditionReference::log() const {
  std::cout << "id: " << m_identifier << "\n";
  node()->log(std::cout, true, 1, true);
}
#endif

Node* EditionReference::node() const {
  return EditionPool::sharedEditionPool()->nodeForIdentifier(m_identifier);
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

void MoveTreeBeforeNode(EditionReference& target, Node* treeToMove) {
  Node* previousTarget = target;
  if (treeToMove->block() < previousTarget->block()) {
    previousTarget =
        Node::FromBlocks(previousTarget->block() - treeToMove->treeSize());
  }
  target->moveTreeBeforeNode(treeToMove);
  target = previousTarget;
}

void SwapTrees(EditionReference& u, EditionReference& v) {
  if (u->block() > v->block()) {
    return SwapTrees(v, u);
  }
  Node* previousU = u;
  Node* previousV = v;
  MoveTreeBeforeNode(v, previousU);
  u = EditionReference(previousU);
  MoveTreeBeforeNode(u, previousV);
}

}  // namespace PoincareJ
