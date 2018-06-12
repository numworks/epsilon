#include "tree_node.h"
#include "expression_node.h"

void TreeNode::release() {
  m_referenceCounter--;
  for (TreeNode * child : directChildren()) {
    child->release();
  }
  if (m_referenceCounter == 0) {
    printf("DELETE %d(%p)\n", m_identifier, this);
    delete this;
    //dealloc();
    printf("Will log\n");
    ExpressionNode::Pool()->log();
  }
}
