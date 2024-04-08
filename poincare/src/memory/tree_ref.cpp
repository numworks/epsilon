#include "tree_ref.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <string.h>

#include "node_iterator.h"
#include "pattern_matching.h"
#include "tree_stack.h"

namespace PoincareJ {

TreeRef::TreeRef(Tree* node) {
  if (!node) {
    m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
    return;
  }
  assert(SharedTreeStack->contains(node->block()) ||
         node->block() == SharedTreeStack->lastBlock());
  m_identifier = SharedTreeStack->referenceNode(node);
}

TreeRef& TreeRef::operator=(Tree* tree) {
  if (!tree) {
    m_identifier = TreeStack::ReferenceTable::NoNodeIdentifier;
  } else if (m_identifier != TreeStack::ReferenceTable::NoNodeIdentifier) {
    SharedTreeStack->updateIdentifier(m_identifier, tree);
  } else {
    m_identifier = SharedTreeStack->referenceNode(tree);
  }
  return *this;
}

#if POINCARE_TREE_LOG
void TreeRef::log() const {
  std::cout << "id: " << m_identifier << "\n";
  tree()->log(std::cout, true, 1, true);
}
#endif

Tree* TreeRef::tree() const {
  return SharedTreeStack->nodeForIdentifier(m_identifier);
}

void TreeRef::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

void CloneNodeAtNode(TreeRef& target, const Tree* nodeToClone) {
  Tree* previousTarget = target;
  target->cloneNodeAtNode(nodeToClone);
  target = previousTarget;
}

void CloneTreeAtNode(TreeRef& target, const Tree* treeToClone) {
  Tree* previousTarget = target;
  target->cloneTreeAtNode(treeToClone);
  target = previousTarget;
}

void MoveAt(TreeRef& target, Tree* source, bool tree, bool before) {
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

}  // namespace PoincareJ
