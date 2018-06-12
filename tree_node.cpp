#include "tree_node.h"
#include "expression_node.h"

void TreeNode::release() {
  printf("release of %d\n", m_identifier);
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    for (TreeNode * child : directChildren()) {
      child->release();
    }

    printf("DELETE %d(%p)\n", m_identifier, this);
    delete this;
  }
}
