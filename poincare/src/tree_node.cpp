#include <poincare/tree_node.h>
#include <poincare/tree_pool.h>
#include <poincare/tree_reference.h>
#include <stdio.h>

namespace Poincare {

bool TreeNode::isStatic() const {
  return m_identifier <= TreePool::FirstStaticNodeIdentifier;
}

// Node operations

void TreeNode::release(int currentNumberOfChildren) {
  if (isStatic()) {
    // Do not release static nodes
    return;
  }
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    releaseChildrenAndDestroy(currentNumberOfChildren);
  }
}

void TreeNode::releaseChildren(int currentNumberOfChildren) {
  for (int i = 0; i < currentNumberOfChildren; i++) {
    TreeRef childRef = TreeRef(childAtIndex(0));
    TreePool::sharedPool()->move(TreePool::sharedPool()->last(), childRef.node(), childRef.numberOfChildren());
    childRef.node()->release(childRef.numberOfChildren());
  }
  eraseNumberOfChildren();
}

void TreeNode::releaseChildrenAndDestroy(int currentNumberOfChildren) {
  releaseChildren(currentNumberOfChildren);
  TreePool::sharedPool()->discardTreeNode(this);
}

void TreeNode::rename(int identifier, bool unregisterPreviousIdentifier) {
  if (unregisterPreviousIdentifier) {
    /* The previous identifier should not always be unregistered. For instance,
     * if the node is a clone and still has the original node's identifier,
     * unregistering it would lose the access to the original node. */
    TreePool::sharedPool()->unregisterNode(this);
  }
  m_identifier = identifier;
  m_referenceCounter = 1;
  TreePool::sharedPool()->registerNode(this);
}

// Hierarchy

TreeNode * TreeNode::parent() const {
  if (isStatic()) {
    return nullptr;
  }
  /* Choose between these algorithms: the first has complexity O(numberNodes)
   * but uses O(3maxNumberNodes) space. The second is much clearer for the
   * reader and uses no space, but has complexity O(numberNodes^2) */
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
  if (isAllocationFailure()) {
    return nullptr;
  }
  assert(false);
  return nullptr;
#endif
}

TreeNode * TreeNode::root() {
  if (isStatic()) {
    return this;
  }
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
  TreeNode * nextSiblingNode = nextSibling();
  TreeNode * currentNode = next();
  while (currentNode != nextSiblingNode) {
    result++;
    currentNode = currentNode->next();
  }
  return result;
}

TreeNode * TreeNode::childAtIndex(int i) const {
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

int TreeNode::indexInParent() const {
  TreeNode * p = parent();
  if (p == nullptr) {
    return -1;
  }
  return p->indexOfChildByIdentifier(m_identifier);
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
  TreeNode * p = parent();
  if (p == nullptr) {
    return false;
  }
  for (TreeNode * childNode : p->directChildren()) {
    if (childNode == e) {
      return true;
    }
  }
  return false;
}

TreeNode::TreeNode() :
  m_identifier(TreePool::NoNodeIdentifier),
  m_referenceCounter(1)
{
}


size_t TreeNode::deepSize(int realNumberOfChildren) const {
  if (realNumberOfChildren == -1) {
    // TODO: Error handling
    return
      reinterpret_cast<char *>(nextSibling())
      -
      reinterpret_cast<const char *>(this);
  }
  TreeNode * realNextSibling = next();
  for (int i = 0; i < realNumberOfChildren; i++) {
    realNextSibling = realNextSibling->nextSibling();
  }
  return
    reinterpret_cast<char *>(realNextSibling)
    -
    reinterpret_cast<const char *>(this);
}


}
