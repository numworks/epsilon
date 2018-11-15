#include <poincare/tree_handle.h>
#include <poincare/ghost.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Clone */

TreeHandle TreeHandle::clone() const {
  assert(!isUninitialized());
  TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(node());
  nodeCopy->deleteParentIdentifier();
  return TreeHandle(nodeCopy);
}

/* Hierarchy operations */
TreeNode * TreeHandle::node() const { assert(m_identifier != TreeNode::NoNodeIdentifier); return TreePool::sharedPool()->node(m_identifier); }

size_t TreeHandle::size() const { return node()->deepSize(node()->numberOfChildren()); }

TreeHandle TreeHandle::parent() const { return (isUninitialized() || node()->parent() == nullptr) ? TreeHandle() : TreeHandle(node()->parent()); }

int TreeHandle::indexOfChild(TreeHandle t) const { return node()->indexOfChild(t.node()); }

bool TreeHandle::hasChild(TreeHandle t) const { return node()->hasChild(t.node()); }

TreeHandle TreeHandle::childAtIndex(int i) const { return TreeHandle(node()->childAtIndex(i)); }

void TreeHandle::replaceWithInPlace(TreeHandle t) {
  assert(!isUninitialized());
  TreeHandle p = parent();
  if (!p.isUninitialized()) {
    p.replaceChildInPlace(*this, t);
  }
}

void TreeHandle::replaceChildInPlace(TreeHandle oldChild, TreeHandle newChild) {
  assert(!oldChild.isUninitialized());
  assert(!newChild.isUninitialized());
  assert(hasChild(oldChild));

  if (oldChild == newChild) {
    return;
  }

  assert(!isUninitialized());

  // If the new child has a parent, detach from it
  newChild.detachFromParent();

  // Move the new child
  assert(newChild.isGhost() || newChild.parent().isUninitialized());
  TreePool::sharedPool()->move(oldChild.node(), newChild.node(), newChild.numberOfChildren());
  newChild.node()->retain();
  newChild.setParentIdentifier(identifier());

  // Move the old child
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), oldChild.node(), oldChild.numberOfChildren());
  oldChild.node()->release(oldChild.numberOfChildren());
  oldChild.deleteParentIdentifier();
}

void TreeHandle::replaceChildAtIndexInPlace(int oldChildIndex, TreeHandle newChild) {
  assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
  TreeHandle oldChild = childAtIndex(oldChildIndex);
  replaceChildInPlace(oldChild, newChild);
}

void TreeHandle::replaceChildWithGhostInPlace(TreeHandle t) {
  Ghost ghost;
  return replaceChildInPlace(t, ghost);
}

void TreeHandle::mergeChildrenAtIndexInPlace(TreeHandle t, int i) {
  /* mergeChildrenAtIndexInPlace should only be called with a tree that can
   * have any number of children, so there is no need to replace the stolen
   * children with ghosts. */
  // TODO assert this and t are "dynamic" trees
  assert(i >= 0 && i <= numberOfChildren());
  // Steal operands
  int numberOfNewChildren = t.numberOfChildren();
  if (i < numberOfChildren()) {
    TreePool::sharedPool()->moveChildren(node()->childAtIndex(i), t.node());
  } else {
    TreePool::sharedPool()->moveChildren(node()->lastDescendant()->next(), t.node());
  }
  node()->incrementNumberOfChildren(numberOfNewChildren);
  t.node()->eraseNumberOfChildren();
  for (int j = 0; j < numberOfNewChildren; j++) {
    assert(i+j < numberOfChildren());
    childAtIndex(i+j).setParentIdentifier(identifier());
  }
  // If t is a child, remove it
  if (node()->hasChild(t.node())) {
    removeChildInPlace(t, 0);
  }
}

void TreeHandle::swapChildrenInPlace(int i, int j) {
  assert(i >= 0 && i < numberOfChildren());
  assert(j >= 0 && j < numberOfChildren());
  if (i == j) {
    return;
  }
  int firstChildIndex = i < j ? i : j;
  int secondChildIndex = i > j ? i : j;
  TreeHandle firstChild = childAtIndex(firstChildIndex);
  TreeHandle secondChild = childAtIndex(secondChildIndex);
  TreePool::sharedPool()->move(firstChild.node()->nextSibling(), secondChild.node(), secondChild.numberOfChildren());
  TreePool::sharedPool()->move(childAtIndex(secondChildIndex).node()->nextSibling(), firstChild.node(), firstChild.numberOfChildren());
}

#if POINCARE_TREE_LOG
void TreeHandle::log() const {
  node()->log(std::cout);
  std::cout << std::endl;
}
#endif

/* Protected */

// Add
void TreeHandle::addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren) {
  assert(!isUninitialized());
  assert(!t.isUninitialized());
  assert(index >= 0 && index <= currentNumberOfChildren);

  // If t has a parent, detach t from it.
  t.detachFromParent();
  assert(t.parent().isUninitialized());

  // Move t
  TreeNode * newChildPosition = node()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  TreePool::sharedPool()->move(newChildPosition, t.node(), t.numberOfChildren());
  t.node()->retain();
  node()->incrementNumberOfChildren();
  t.setParentIdentifier(identifier());

  node()->didAddChildAtIndex(currentNumberOfChildren+1);
}

// Remove

void TreeHandle::removeChildAtIndexInPlace(int i) {
  assert(!isUninitialized());
  assert(i >= 0 && i < numberOfChildren());
  TreeHandle t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());
}

void TreeHandle::removeChildInPlace(TreeHandle t, int childNumberOfChildren) {
  assert(!isUninitialized());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  t.deleteParentIdentifier();
  node()->decrementNumberOfChildren();
}

void TreeHandle::removeChildrenInPlace(int currentNumberOfChildren) {
  assert(!isUninitialized());
  deleteParentIdentifierInChildren();
  TreePool::sharedPool()->removeChildren(node(), currentNumberOfChildren);
}

/* Private */

void TreeHandle::detachFromParent() {
  TreeHandle myParent = parent();
  if (!myParent.isUninitialized()) {
    int idxInParent = myParent.indexOfChild(*this);
    myParent.replaceChildAtIndexWithGhostInPlace(idxInParent);
  }
  assert(parent().isUninitialized());
}


TreeHandle::TreeHandle(const TreeNode * node) : TreeHandle() {
  if (node != nullptr) {
    setIdentifierAndRetain(node->identifier());
  }
}

void TreeHandle::setIdentifierAndRetain(int newId) {
  m_identifier = newId;
  if (!isUninitialized()) {
    node()->retain();
  }
}

void TreeHandle::setTo(const TreeHandle & tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * TreeHandle, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  int currentId = identifier();
  setIdentifierAndRetain(tr.identifier());
  release(currentId);
}

void TreeHandle::release(int identifier) {
  if (identifier == TreeNode::NoNodeIdentifier) {
    return;
  }
  TreeNode * node = TreePool::sharedPool()->node(identifier);
  if (node == nullptr) {
    /* The identifier is valid, but not the node: there must have been an
     * exception that deleted the pool. */
    return;
  }
  assert(node->identifier() == identifier);
  node->release(node->numberOfChildren());
}

}
