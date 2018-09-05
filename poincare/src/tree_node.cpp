#include <poincare/tree_node.h>
#include <poincare/tree_pool.h>
#include <poincare/tree_by_reference.h>
#include <stdio.h>

namespace Poincare {

// Node operations

void TreeNode::deleteParentIdentifier() {
  m_parentIdentifier = TreePool::NoNodeIdentifier;
}

void TreeNode::release(int currentNumberOfChildren) {
  if (isStatic()) {
    // Do not release static nodes
    return;
  }
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    deleteParentIdentifierInChildren();
    TreePool::sharedPool()->removeChildrenAndDestroy(this, currentNumberOfChildren);
  }
}

void TreeNode::rename(int identifier, bool unregisterPreviousIdentifier) {
  if (unregisterPreviousIdentifier) {
    /* The previous identifier should not always be unregistered. For instance,
     * if the node is a clone and still has the original node's identifier,
     * unregistering it would lose the access to the original node. */
    TreePool::sharedPool()->unregisterNode(this);
  }
  m_identifier = identifier;
  m_referenceCounter = 0;
  TreePool::sharedPool()->registerNode(this);
  updateParentIdentifierInChildren();
}

// Hierarchy

TreeNode * TreeNode::parent() const {
  if (m_parentIdentifier == TreePool::NoNodeIdentifier) {
    return nullptr;
  }
  return TreePool::sharedPool()->node(m_parentIdentifier);
}

TreeNode * TreeNode::root() {
  TreeNode * result = this;
  TreeNode * resultParent = result->parent();
  while(resultParent != nullptr) {
    result = resultParent;
    resultParent = result->parent();
  }
  return result;
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

int TreeNode::indexOfChild(const TreeNode * child) const {
  assert(child != nullptr);
  int childrenCount = numberOfChildren();
  TreeNode * childAtIndexi = next();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi == child) {
      return i;
    }
    childAtIndexi = childAtIndexi->nextSibling();
  }
  return -1;
}

int TreeNode::indexInParent() const {
  TreeNode * p = parent();
  if (p == nullptr) {
    return -1;
  }
  return p->indexOfChild(this);
}

bool TreeNode::hasChild(const TreeNode * child) const {
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

TreeNode * TreeNode::nextSibling() const {
  int remainingNodesToVisit = numberOfChildren();
  TreeNode * node = const_cast<TreeNode *>(this)->next();
  while (remainingNodesToVisit > 0) {
    remainingNodesToVisit += node->numberOfChildren();
    node = node->next();
    remainingNodesToVisit--;
  }
  return node;
}

TreeNode * TreeNode::lastDescendant() const {
  TreeNode * node = const_cast<TreeNode *>(this);
  int remainingNodesToVisit = node->numberOfChildren();
  while (remainingNodesToVisit > 0) {
    node = node->next();
    remainingNodesToVisit--;
    remainingNodesToVisit += node->numberOfChildren();
  }
  return node;
}

// Protected

TreeNode::TreeNode() :
  m_identifier(TreePool::NoNodeIdentifier),
  m_parentIdentifier(TreePool::NoNodeIdentifier),
  m_referenceCounter(0)
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

void TreeNode::deleteParentIdentifierInChildren() const {
  changeParentIdentifierInChildren(TreePool::NoNodeIdentifier);
}

void TreeNode::changeParentIdentifierInChildren(int id) const {
  for (TreeNode * c : directChildren()) {
    c->setParentIdentifier(id);
  }
}

}
