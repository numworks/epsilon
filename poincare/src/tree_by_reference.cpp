#include <poincare/tree_by_reference.h>
#include <poincare/ghost_reference.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Constructors */

TreeByReference::~TreeByReference() {
  if (isDefined()) {
    assert(node()->identifier() == m_identifier);
    node()->release(numberOfChildren()); //TODO No malformed nodes ?
  }
}

/* Clone */

TreeByReference TreeByReference::clone() const {
  if (!isDefined()){
    return TreeByReference();
  }
  TreeNode * myNode = node();
  if (myNode->isAllocationFailure()) {
    int allocationFailureNodeId = myNode->allocationFailureNodeIdentifier();
    return TreeByReference(TreePool::sharedPool()->node(allocationFailureNodeId));
  }
  TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(myNode);
  return TreeByReference(nodeCopy);
}

/* Hierarchy operations */

void TreeByReference::replaceWithInPlace(TreeByReference t) {
  assert(isDefined());
  if (isAllocationFailure()) {
    return;
  }
  TreeByReference p = parent();
  if (p.isDefined()) {
    p.replaceChildInPlace(*this, t);
  }
}

void TreeByReference::replaceChildInPlace(TreeByReference oldChild, TreeByReference newChild) {
  if (oldChild == newChild) {
    return;
  }

  assert(isDefined());
  if (isAllocationFailure()) {
    return;
  }
  if (newChild.isAllocationFailure()) {
    replaceWithAllocationFailureInPlace(numberOfChildren());
    return;
  }

  // Move the new child
  assert(!newChild.parent().isDefined());
  TreePool::sharedPool()->move(oldChild.node(), newChild.node(), newChild.numberOfChildren());
  /* We could have moved the new node to oldChild.node()->nextSibling(), but
   * nextSibling is not computed correctly if we inserted an
   * AllocationFailureNode next to newChild. */
  newChild.node()->retain();

  // Move the old child
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), oldChild.node(), oldChild.numberOfChildren());
  oldChild.node()->release(oldChild.numberOfChildren());
}

void TreeByReference::replaceWithAllocationFailureInPlace(int currentNumberOfChildren) {
  if (isAllocationFailure()) {
    return;
  }
  assert(isDefined());
  TreeByReference p = parent();
  bool hasParent = p.isDefined();
  int indexInParentNode = hasParent ? node()->indexInParent() : -1;
  int currentRetainCount = node()->retainCount();
  TreeNode * staticAllocFailNode = node()->failedAllocationStaticNode();

  // Release all children and delete the node in the pool
  removeChildrenAndDestroyInPlace(currentNumberOfChildren);
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
  newAllocationFailureNode->retain();
  if (p.isDefined()) {
    assert(indexInParentNode >= 0);
    /* Set the refCount to previousRefCount-1 because the previous parent is
     * no longer retaining the node. When we add this node to the parent, it
     * will retain it and increment the retain count. */
    newAllocationFailureNode->setReferenceCounter(currentRetainCount - 1);
    p.addChildAtIndexInPlace(TreeByReference(newAllocationFailureNode), indexInParentNode, p.numberOfChildren() - 1);
    p.decrementNumberOfChildren();
    /* We decrement here the parent's number of children, as we did not do it
     * before, see WARNING. */
  } else {
    newAllocationFailureNode->setReferenceCounter(currentRetainCount);
  }
}

void TreeByReference::replaceChildWithGhostInPlace(TreeByReference t) {
  GhostReference ghost;
  return replaceChildInPlace(t, ghost);
}

void TreeByReference::mergeChildrenAtIndexInPlace(TreeByReference t, int i) {
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
    removeChildInPlace(t, 0);
  }
  node()->incrementNumberOfChildren(numberOfNewChildren);
}

void TreeByReference::swapChildrenInPlace(int i, int j) {
  assert(isDefined());
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
  assert(isDefined());
  if (node()->isAllocationFailure()) {
    return;
  }
  if (t.isAllocationFailure()) {
    replaceWithAllocationFailureInPlace(currentNumberOfChildren);
    return;
  }
  assert(index >= 0 && index <= currentNumberOfChildren);

  assert(!t.parent().isDefined());
  // Move t
  TreeNode * newChildPosition = node()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  TreePool::sharedPool()->move(newChildPosition, t.node(), t.numberOfChildren());
  t.node()->retain();
  node()->incrementNumberOfChildren();

  node()->didAddChildAtIndex(numberOfChildren());
}

// Remove

void TreeByReference::removeChildAtIndexInPlace(int i) {
  assert(isDefined());
  assert(i >= 0 && i < numberOfChildren());
  TreeByReference t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());
}

void TreeByReference::removeChildInPlace(TreeByReference t, int childNumberOfChildren) {
  assert(isDefined());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  node()->decrementNumberOfChildren();
}

void TreeByReference::removeChildrenInPlace(int currentNumberOfChildren) {
  assert(isDefined());
  for (int i = 0; i < currentNumberOfChildren; i++) {
    TreeByReference childRef = childAtIndex(0);
    TreePool::sharedPool()->move(TreePool::sharedPool()->last(), childRef.node(), childRef.numberOfChildren());
    childRef.node()->release(childRef.numberOfChildren());
  }
  node()->eraseNumberOfChildren();
}

void TreeByReference::removeChildrenAndDestroyInPlace(int currentNumberOfChildren) {
  removeChildrenInPlace(currentNumberOfChildren);
  TreePool::sharedPool()->discardTreeNode(node());
}

/* Private */

void TreeByReference::setTo(const TreeByReference & tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * TreeByReference, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  TreeNode * currentNode = node();
  setIdentifierAndRetain(tr.identifier());
  currentNode->release(currentNode->numberOfChildren());
}

}
