#include "tree_node.h"
#include "expression_node.h"

void TreeNode::release() {
  printf("releasing of %d(%p)\n", m_identifier, this);
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
#if 0
    for (TreeNode * child : directChildren()) {
      // BUG FIXME
      // Here, if we end up delete-ing the node
      // then the 'next child' is wrong...
      child->release();
    }
#endif
    if (numberOfChildren() != 0) {
      int lastIdentifier = lastDescendant()->identifier();
      TreeNode * child = this;
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

    printf("DELETE %d(%p)\n", m_identifier, this);
    delete this;
  }
}
