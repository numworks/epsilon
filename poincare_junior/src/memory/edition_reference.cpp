#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <string.h>

#include "edition_pool.h"
#include "node_iterator.h"
#include "pattern_matching.h"

namespace PoincareJ {

EditionReference::EditionReference(Tree* node) {
  if (!node) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  assert(SharedEditionPool->contains(node->block()) ||
         node->block() == SharedEditionPool->lastBlock());
  m_identifier = SharedEditionPool->referenceNode(node);
}

EditionReference& EditionReference::operator=(Tree* tree) {
  if (!tree) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
  } else if (m_identifier == EditionPool::ReferenceTable::NoNodeIdentifier ||
             !SharedEditionPool->updateIdentifier(m_identifier, tree)) {
    m_identifier = SharedEditionPool->referenceNode(tree);
  }
  return *this;
}

#if POINCARE_MEMORY_TREE_LOG
void EditionReference::log() const {
  std::cout << "id: " << m_identifier << "\n";
  tree()->log(std::cout, true, 1, true);
}
#endif

Tree* EditionReference::tree() const {
  return SharedEditionPool->nodeForIdentifier(m_identifier);
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

void CloneNodeAtNode(EditionReference& target, const Tree* nodeToClone) {
  Tree* previousTarget = target;
  target->cloneNodeAtNode(nodeToClone);
  target = previousTarget;
}

void CloneTreeAtNode(EditionReference& target, const Tree* treeToClone) {
  Tree* previousTarget = target;
  target->cloneTreeAtNode(treeToClone);
  target = previousTarget;
}

void MoveAt(EditionReference& target, Tree* source, bool tree, bool before) {
  Tree* previousTarget = target;
  if (source->block() < previousTarget->block()) {
    previousTarget =
        Tree::FromBlocks(previousTarget->block() -
                         (tree ? source->treeSize() : source->nodeSize()));
  }
  if (tree) {
    if (before) {
      target->moveTreeBeforeNode(source);
    } else {
      target->moveTreeAtNode(source);
    }
  } else {
    if (before) {
      target->moveNodeBeforeNode(source);
    } else {
      target->moveNodeAtNode(source);
    }
  }
  target = previousTarget;
}

void SwapTrees(EditionReference& u, EditionReference& v) {
  if (u->block() > v->block()) {
    return SwapTrees(v, u);
  }
  Tree* previousU = u;
  Tree* previousV = v;
  MoveTreeAtNode(v, previousU);
  u = EditionReference(previousU);
  MoveTreeAtNode(u, previousV);
}

}  // namespace PoincareJ
