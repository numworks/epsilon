#include "tree_node.h"
#include "tree_pool.h"
#include "expression_node.h"
#include <stdio.h>

// Node operations

void TreeNode::release() {
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
    TreePool::sharedPool()->discardTreeNode(this);
  }
}

// Hierarchy

TreeNode * TreeNode::parentTree() const {
// Choose between those two algorithms: the first has complexity O(numberNodes) but uses 0(3maxNumberNodes) space
// The second is much clearer for the reader and uses no space, but has complexity 0
#if 0
  int cursor = -1;
  TreeNode * parentsHistory[TreePool::MaxNumberOfNodes];
  int numberOfChildrenHistory[TreePool::MaxNumberOfNodes];
  int childrenVisitedCountHistory[TreePool::MaxNumberOfNodes];
  for (TreeNode * node : TreePool::sharedPool()->allNodes()) {
    if (node->identifier() == m_identifier) {
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

TreeNode * TreeNode::editableRootTree() {
  for (TreeNode * root : TreePool::sharedPool()->roots()) {
    if (hasAncestor(root, true)) {
      return root;
    }
  }
  assert(false);
  return nullptr;
}

int TreeNode::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  for (TreeNode * child : depthFirstChildren()) {
    result++;
  }
  return result;
}

TreeNode * TreeNode::childTreeAtIndex(int i) const {
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

int TreeNode::indexOfChildByIdentifier(int childID) const {
  int childrenCount = numberOfChildren();
  TreeNode * childAtIndexi = next();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi->identifier() == childID) {
      return i;
    }
    childAtIndexi = childAtIndexi->nextSibling();
  }
  return -1;
}

int TreeNode::indexOfChild(const TreeNode * child) const {
  if (child == nullptr) {
    return -1;
  }
  return indexOfChildByIdentifier(child->identifier());
}

bool TreeNode::hasChild(const TreeNode * child) const {
  if (child == nullptr) {
    return false;
  }
  for (TreeNode * c : directChildren()) {
    if (child == c) {
      return true;
    }
  }
  return false;
}

bool TreeNode::hasAncestor(const TreeNode * node, bool includeSelf) const {
  if (includeSelf && node == this) {
    return true;
  }
  for (TreeNode * t : node->depthFirstChildren()) {
    if (this == t) {
      return true;
    }
  }
  return false;
}

bool TreeNode::hasSibling(const TreeNode * e) const {
  if (e == nullptr) {
    return false;
  }
  TreeNode * parent = parentTree();
  if (parent == nullptr) {
    return false;
  }
  for (TreeNode * childNode : parent->directChildren()) {
    if (childNode == e) {
      return true;
    }
  }
  return false;
}
