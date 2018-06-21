#include "tree_node.h"
#include "tree_pool.h"
#include "expression_node.h"

void TreeNode::release() {
  printf("Release %d(%p)\n", m_identifier, this);
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    if (numberOfChildren() != 0) {
      int lastIdentifier = lastDescendant()->identifier();
      TreeNode * child = next();
      do {
        bool childWillBeDeleted = (child->m_referenceCounter == 1);
        child->release();
        if (!childWillBeDeleted) {
          printf("Incrementing iterator\n");
          child = child->next();
        } else {
          printf("Keeping iterator\n");
        }
      } while (child->identifier() != lastIdentifier);
    }
    printf("Delete %d(%p)\n", m_identifier, this);
    int identifier = m_identifier;
    delete this;
    TreePool::sharedPool()->freeIdentifier(identifier);
  }
}
