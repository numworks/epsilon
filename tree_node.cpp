#include "tree_node.h"
#include "tree_pool.h"
#include "expression_node.h"

TreeNode * TreeNode::parent() const {
  int cursor = -1;
  TreeNode * parentsHistory[TreePool::MaxNumberOfNodes];
  int numberOfChildrenHistory[TreePool::MaxNumberOfNodes];
  int childrenVisitedCountHistory[TreePool::MaxNumberOfNodes];
  for (TreeNode * node : TreePool::sharedPool()->allNodes()) {
    if (node->identifier() == m_identifier) {
      printf("Parent of %d is %d\n", m_identifier, cursor >= 0 ? parentsHistory[cursor]->identifier() : -1);
      return cursor >= 0 ? parentsHistory[cursor] : nullptr;
    }
    if (cursor >= 0) {
      childrenVisitedCountHistory[cursor] = childrenVisitedCountHistory[cursor]+1;
    }
    int nodeChildrenCount = node->numberOfChildren();
    if (nodeChildrenCount > 0) {
      cursor++;
      parentsHistory[cursor] = node;
      numberOfChildrenHistory[cursor] = nodeChildrenCount;
      childrenVisitedCountHistory[cursor] = 0;
    } else {
      if (cursor >= 0 && (numberOfChildrenHistory[cursor] == childrenVisitedCountHistory[cursor])) {
        cursor--;
      }
    }
  }
  assert(false);
  return nullptr;
}

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
    TreePool::sharedPool()->discardTreeNode(this);
  }
}
