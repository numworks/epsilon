#include <poincare/tree_reference.h>

namespace Poincare {

TreeReference TreeReference::treeClone() const {
  if (!isDefined()){
    return TreeReference(nullptr);
  }
  TreeNode * myNode = node();
  if (myNode->isAllocationFailure()) {
    int allocationFailureNodeId = myNode->allocationFailureNodeIdentifier();
    return TreeReference(TreePool::sharedPool()->node(allocationFailureNodeId));
  }
  TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(myNode);
  nodeCopy->deepResetReferenceCounter();
  return TreeReference(nodeCopy);
}

TreeReference::~TreeReference() {
  if (isDefined()) {
    assert(node()->identifier() == m_identifier);
    node()->release(numberOfChildren()); //TODO No malformed nodes ?
  }
}

// Hierarchy operations

// Detach

void TreeReference::detachChild(TreeReference t, int childNumberOfChildren) {
  assert(isDefined());
  // Replace the child with a ghost node
  TreeNode * staticGhostNode = t.node()->ghostStaticNode();
  TreeNode * newGhostNode = TreePool::sharedPool()->deepCopy(staticGhostNode);
  if (newGhostNode == nullptr) {
    t.replaceWithAllocationFailure(t.numberOfChildren());
    return;
  }

  // Put the ghost next to the child to detach
  newGhostNode->retain();
  TreePool::sharedPool()->move(t.node(), newGhostNode, 0);

  // Move the child to detach
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), t.numberOfChildren());
  t.node()->release(childNumberOfChildren);
}

void TreeReference::detachFromParent() {
  assert(isDefined());
  TreeReference p = parent();
  if (!p.isDefined()) {
    return;
  }
  p.detachChild(*this, numberOfChildren());
}

void TreeReference::replaceWith(TreeReference t) {
  assert(isDefined());
  TreeReference p = parent();
  if (p.isDefined()) {
    p.replaceTreeChild(*this, t);
  }
}

void TreeReference::replaceTreeChild(TreeReference oldChild, TreeReference newChild) {
  if (oldChild == newChild) {
    return;
  }

  assert(isDefined());
  if (newChild.isAllocationFailure()) {
    replaceWithAllocationFailure(numberOfChildren());
    return;
  }

  // Move the new child
  newChild.detachOrRemoveFromParent();
  TreePool::sharedPool()->move(oldChild.node(), newChild.node(), newChild.numberOfChildren());
  /* We could have moved the new node to oldChild.node()->nextSibling(), but
   * nextSibling is not computed correctly if we inserted an
   * AllocationFailureNode next to newChild. */
  newChild.node()->retain();

  // Move the old child
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), oldChild.node(), oldChild.numberOfChildren());
  oldChild.node()->release(oldChild.numberOfChildren());
}

void TreeReference::swapChildren(int i, int j) {
  assert(isDefined());
  assert(i >= 0 && i < numberOfChildren());
  assert(j >= 0 && j < numberOfChildren());
  if (i == j) {
    return;
  }
  int firstChildIndex = i < j ? i : j;
  int secondChildIndex = i > j ? i : j;
  TreeReference firstChild = treeChildAtIndex(firstChildIndex);
  TreeReference secondChild = treeChildAtIndex(secondChildIndex);
  TreePool::sharedPool()->move(firstChild.node()->nextSibling(), secondChild.node(), secondChild.numberOfChildren());
  TreePool::sharedPool()->move(treeChildAtIndex(secondChildIndex).node()->nextSibling(), firstChild.node(), firstChild.numberOfChildren());
}

void TreeReference::replaceWithAllocationFailure(int currentNumberOfChildren) {
  if (isAllocationFailure()) {
    return;
  }
  assert(isDefined());
  TreeReference p = parent();
  bool hasParent = p.isDefined();
  int indexInParentNode = hasParent ? node()->indexInParent() : -1;
  int currentRetainCount = node()->retainCount();
  TreeNode * staticAllocFailNode = node()->failedAllocationStaticNode();

  // Release all children and delete the node in the pool
  removeChildrenAndDestroy(currentNumberOfChildren);
  /* WARNING: If we called "p.decrementNumberOfChildren()" here, the number of
   * children of the parent layout would be:
   * -> numberOfChildren() for "dynamic trees" that have a m_numberOfChildren
   * variable (such as HorizontalLayout)
   * -> numberOfChildren() - 1 for "static trees" that have a fixed number of
   * children (such as IntegralLayout)
   *
   * By not decrementing the parent's number of children here, we now that it
   * has (numberOfChildren() - 1) children. */

  /* Create an allocation failure node with the previous node id. We know
   * there is room in the pool as we deleted the previous node and an
   * AllocationFailure nodes size is smaller or equal to any other node size.*/
  //TODO static assert that the size is smaller
  TreeNode * newAllocationFailureNode = TreePool::sharedPool()->deepCopy(staticAllocFailNode);
  newAllocationFailureNode->rename(m_identifier, true);
  if (p.isDefined()) {
    assert(indexInParentNode >= 0);
    /* Set the refCount to previousRefCount-1 because the previous parent is
     * no longer retaining the node. When we add this node to the parent, it
     * will retain it and increment the retain count. */
    newAllocationFailureNode->setReferenceCounter(currentRetainCount - 1);
    p.addChildAtIndex(TreeRef(newAllocationFailureNode), indexInParentNode, p.numberOfChildren() - 1);
    p.decrementNumberOfChildren();
    /* We decrement here the parent's number of children, as we did not do it
     * before, see WARNING. */
  } else {
    newAllocationFailureNode->setReferenceCounter(currentRetainCount);
  }
}

void TreeReference::mergeTreeChildrenAtIndex(TreeReference t, int i) {
  assert(i >= 0 && i <= numberOfChildren());
  // Steal operands
  int numberOfNewChildren = t.numberOfChildren();
  if (i < numberOfChildren()) {
    TreePool::sharedPool()->moveChildren(node()->childAtIndex(i), t.node());
  } else {
    TreePool::sharedPool()->moveChildren(node()->lastDescendant()->next(), t.node());
  }
  t.node()->eraseNumberOfChildren();
  // If t is a child, remove it
  if (node()->hasChild(t.node())) {
    removeChild(t, 0);
  }
  node()->incrementNumberOfChildren(numberOfNewChildren);
}

// Protected
void TreeReference::setTo(const TreeReference & tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * TreeReference, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  TreeNode * currentNode = node();
  bool releaseNode = isDefined();
  if (tr.isDefined()) {
    setIdentifierAndRetain(tr.identifier());
  } else {
    m_identifier = TreePool::NoNodeIdentifier;
  }
  if (releaseNode) {
    currentNode->release(currentNode->numberOfChildren());
  }
}

// Add
void TreeReference::addChildAtIndex(TreeReference t, int index, int currentNumberOfChildren) {
  assert(isDefined());
  if (node()->isAllocationFailure()) {
    return;
  }
  if (t.isAllocationFailure()) {
    replaceWithAllocationFailure(currentNumberOfChildren);
    return;
  }
  assert(index >= 0 && index <= currentNumberOfChildren);

  // Detach or remove t from its parent
  t.detachOrRemoveFromParent();

  // Move t
  TreeNode * newChildPosition = node()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  TreePool::sharedPool()->move(newChildPosition, t.node(), t.numberOfChildren());
  t.node()->retain();
  node()->incrementNumberOfChildren();
}

// Remove

void TreeReference::removeChildAtIndex(int i) {
  assert(isDefined());
  assert(i >= 0 && i < numberOfChildren());
  TreeReference t = treeChildAtIndex(i);
  removeChild(t, t.numberOfChildren());
}

void TreeReference::removeChild(TreeReference t, int childNumberOfChildren) {
  assert(isDefined());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  node()->decrementNumberOfChildren();
}

void TreeReference::removeChildren(int currentNumberOfChildren) {
  assert(isDefined());
  for (int i = 0; i < currentNumberOfChildren; i++) {
    TreeReference childRef = treeChildAtIndex(0);
    TreePool::sharedPool()->move(TreePool::sharedPool()->last(), childRef.node(), childRef.numberOfChildren());
    childRef.node()->release(childRef.numberOfChildren());
  }
  node()->eraseNumberOfChildren();
}

void TreeReference::removeChildrenAndDestroy(int currentNumberOfChildren) {
  removeChildren(currentNumberOfChildren);
  TreePool::sharedPool()->discardTreeNode(node());
}

void TreeReference::removeFromParent() {
  assert(isDefined());
  TreeReference p = parent();
  if (!p.isDefined()) {
    return;
  }
  p.removeChild(*this, numberOfChildren());
}

// Private
void TreeReference::detachOrRemoveFromParent() {
  assert(isDefined());
  TreeReference p = parent();
  if (p.isDefined()) {
    if (p.isChildRemovalTolerant()) {
      p.removeChild(*this, numberOfChildren());
    } else {
      p.detachChild(*this, numberOfChildren());
    }
  }
}

}
