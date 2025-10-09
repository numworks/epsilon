#include "tree_ref.h"

#include <omg/code_point.h>
#include <poincare/src/expression/k_tree.h>
#include <string.h>

#include "pattern_matching.h"
#include "tree_stack.h"

namespace Poincare::Internal {

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

}  // namespace Poincare::Internal
