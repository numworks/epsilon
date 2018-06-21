#include "tree_node.h"
#include "tree_pool.h"
#include "expression_node.h"

void TreeNode::release() {
  printf("Release %d(%p)\n", m_identifier, this);
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    if (numberOfChildren() != 0) {
      int lastIdentifier = lastChild()->identifier();
      TreeNode * child = next();
      bool lastChildReleased = false;
      while (!lastChildReleased) {
        lastChildReleased = child->identifier() == lastIdentifier;
        int nextSiblingIdentifier = lastChildReleased ? -1 : child->nextSibling()->identifier();
        child->release();
        if (nextSiblingIdentifier != -1) {
          child = TreePool::sharedPool()->node(nextSiblingIdentifier);
        }
      }
    }
    printf("Delete %d(%p)\n", m_identifier, this);
    int identifier = m_identifier;
    delete this;
    TreePool::sharedPool()->freeIdentifier(identifier);
  }
}
