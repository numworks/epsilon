#include "tree_node.h"
#include "tree_pool.h"
#include "expression_node.h"

TreeNode * TreeNode::treeParent() const {
// Choose between those two algorithms: the first has complexity O(numberNodes) but uses 0(3maxNumberNodes) space
// The second is much clearer for the reader and uses no space, but has complexity 0
#if 0
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
#else
  for (TreeNode * node : TreePool::sharedPool()->allNodes()) {
    if (node == this) {
      return nullptr;
    }
    if (node->hasChild(this)) {
      return node;
    }
  }
  assert(false);
  return nullptr;
#endif
}

int TreeNode::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  for (TreeNode * child : depthFirstChildren()) {
    result++;
  }
  return result;
}

TreeNode * TreeNode::treeChildAtIndex(int i) const {
  assert(i >= 0);
  assert(i < numberOfChildren());
  TreeNode * child = next();
  while (i > 0) {
    child = child->nextSibling();
    assert(child != nullptr);
    i--;
  }
  return child;
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
