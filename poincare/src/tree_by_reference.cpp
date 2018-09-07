#include <poincare/tree_by_reference.h>
#include <poincare/ghost_reference.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Constructors */
TreeByReference::~TreeByReference() {
  if (m_identifier != TreeNode::NoNodeIdentifier) {
    assert(node()->identifier() == m_identifier);
    TreeNode * n = node();
    n->release(n->numberOfChildren());
  }
}

/* Clone */

TreeByReference TreeByReference::clone() const {
#if POINCARE_ALLOW_STATIC_NODES
  if (isStatic()) {
    // Static nodes are not copied
    return TreeByReference(node());
  }
#endif
  /* TODO Remove ?
  if (isUninitialized()) {
    return TreeByReference();
  }*/
  TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(node());
  nodeCopy->deleteParentIdentifier();
  return TreeByReference(nodeCopy);
}

/* Hierarchy operations */

TreeByReference TreeByReference::childAtIndex(int i) const { return TreeByReference(node()->childAtIndex(i)); }

void TreeByReference::replaceWithInPlace(TreeByReference t) {
  assert(!isUninitialized());
  TreeByReference p = parent();
  if (!p.isUninitialized()) {
    p.replaceChildInPlace(*this, t);
  }
}

void TreeByReference::replaceChildInPlace(TreeByReference oldChild, TreeByReference newChild) {
  assert(!oldChild.isUninitialized());
  assert(!newChild.isUninitialized());

  if (oldChild == newChild) {
    return;
  }

  assert(!isUninitialized());

#if POINCARE_ALLOW_STATIC_NODES
  // If the new node is static, copy it in the pool and add the copy
  if (newChild.isStatic()) {
    TreeByReference newT = TreeByReference(TreePool::sharedPool()->deepCopy(newChild.node()));
    replaceChildInPlace(oldChild, newT);
    return;
  }
#endif

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

void TreeByReference::replaceChildAtIndexInPlace(int oldChildIndex, TreeByReference newChild) {
  assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
  TreeByReference oldChild = childAtIndex(oldChildIndex);
  replaceChildInPlace(oldChild, newChild);
}

void TreeByReference::replaceChildWithGhostInPlace(TreeByReference t) {
  GhostReference ghost;
  return replaceChildInPlace(t, ghost);
}

void TreeByReference::mergeChildrenAtIndexInPlace(TreeByReference t, int i) {
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

void TreeByReference::swapChildrenInPlace(int i, int j) {
#if POINCARE_ALLOW_STATIC_NODES
  assert(!isStatic());
#endif
  assert(i >= 0 && i < numberOfChildren());
  assert(j >= 0 && j < numberOfChildren());
  if (i == j) {
    return;
  }
  int firstChildIndex = i < j ? i : j;
  int secondChildIndex = i > j ? i : j;
  TreeByReference firstChild = childAtIndex(firstChildIndex);
  TreeByReference secondChild = childAtIndex(secondChildIndex);
  TreePool::sharedPool()->move(firstChild.node()->nextSibling(), secondChild.node(), secondChild.numberOfChildren());
  TreePool::sharedPool()->move(childAtIndex(secondChildIndex).node()->nextSibling(), firstChild.node(), firstChild.numberOfChildren());
}

#if POINCARE_TREE_LOG
void TreeByReference::log() const {
  node()->log(std::cout);
  std::cout << std::endl;
}
#endif

/* Protected */

// Add
void TreeByReference::addChildAtIndexInPlace(TreeByReference t, int index, int currentNumberOfChildren) {
  assert(!isUninitialized());
  assert(!t.isUninitialized());
  assert(index >= 0 && index <= currentNumberOfChildren);

#if POINCARE_ALLOW_STATIC_NODES
  // If the new node is static, copy it in the pool and add the copy
  if (t.isStatic()) {
    TreeByReference newT = TreeByReference(TreePool::sharedPool()->deepCopy(t.node()));
    addChildAtIndexInPlace(newT, index, currentNumberOfChildren);
    return;
  }
#endif
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

void TreeByReference::removeChildAtIndexInPlace(int i) {
  assert(!isUninitialized());
  assert(i >= 0 && i < numberOfChildren());
  TreeByReference t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());
}

void TreeByReference::removeChildInPlace(TreeByReference t, int childNumberOfChildren) {
  assert(!isUninitialized());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  t.deleteParentIdentifier();
  node()->decrementNumberOfChildren();
}

void TreeByReference::removeChildrenInPlace(int currentNumberOfChildren) {
  assert(!isUninitialized());
  deleteParentIdentifierInChildren();
  TreePool::sharedPool()->removeChildren(node(), currentNumberOfChildren);
}

/* Private */

void TreeByReference::detachFromParent() {
  TreeByReference myParent = parent();
  if (!myParent.isUninitialized()) {
    int idxInParent = myParent.indexOfChild(*this);
    myParent.replaceChildAtIndexWithGhostInPlace(idxInParent);
  }
  assert(parent().isUninitialized());
}


TreeByReference::TreeByReference(const TreeNode * node) : TreeByReference() {
  if (node != nullptr) {
    setIdentifierAndRetain(node->identifier());
  }
}

void TreeByReference::setIdentifierAndRetain(int newId) {
  m_identifier = newId;
  if (!isUninitialized()) {
    node()->retain();
  }
}

void TreeByReference::setTo(const TreeByReference & tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * TreeByReference, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  int currentId = identifier();
  setIdentifierAndRetain(tr.identifier());
  if (currentId != TreeNode::NoNodeIdentifier) {
    TreeNode * previousNode = TreePool::sharedPool()->node(currentId);
    if (previousNode != nullptr) {
      // The node might have been deleted during an exception
      previousNode->release(previousNode->numberOfChildren());
    }
  }
}

}
