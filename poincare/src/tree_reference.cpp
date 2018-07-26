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
    assert(node());
    assert(node()->identifier() == m_identifier);
    node()->release();
  }
}

// Hierarchy operations

void TreeReference::addChildTreeAtIndex(TreeReference t, int index) {
  assert(isDefined());
  if (node()->isAllocationFailure()) {
    return;
  }
  if (t.isAllocationFailure()) {
    replaceWithAllocationFailure();
    return;
  }
  assert(index >= 0 && index <= numberOfChildren());

  // Retain t before detaching it, else it might get destroyed
  t.node()->retain();

  // Detach t from its parent
  TreeReference tParent = t.parent();
  if (tParent.isDefined()) {
    tParent.removeTreeChild(t);
  }

  // Move t
  TreeNode * newChildPosition = node()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  TreePool::sharedPool()->move(newChildPosition, t.node());
  node()->incrementNumberOfChildren();
}

void TreeReference::removeTreeChildAtIndex(int i) {
  assert(isDefined());
  assert(i >= 0 && i < numberOfChildren());
  TreeReference t = treeChildAtIndex(i);
  removeTreeChild(t);
}

void TreeReference::removeTreeChild(TreeReference t) {
  assert(isDefined());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node());
  t.node()->release();
  node()->decrementNumberOfChildren();
}

void TreeReference::removeChildren() {
  assert(isDefined());
  node()->releaseChildren();
  TreePool::sharedPool()->moveChildren(TreePool::sharedPool()->last(), node());
  node()->eraseNumberOfChildren();
}

void TreeReference::replaceWith(TreeReference t) {
  assert(isDefined());
  TreeReference p = parent();
  if (p.isDefined()) {
    p.replaceTreeChildAtIndex(p.node()->indexOfChildByIdentifier(identifier()), t);
  }
}

void TreeReference::replaceTreeChildAtIndex(int oldChildIndex, TreeReference newChild) {
  assert(isDefined());
  if (newChild.isAllocationFailure()) {
    replaceWithAllocationFailure();
    return;
  }
  TreeReference p = newChild.parent();
  if (p.isDefined()) {
    p.decrementNumberOfChildren();
  }
  assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
  TreeReference oldChild = treeChildAtIndex(oldChildIndex);
  TreePool::sharedPool()->move(oldChild.node()->nextSibling(), newChild.node());
  if (!p.isDefined()) {
    newChild.node()->retain();
  }
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), oldChild.node());
  oldChild.node()->release();
}

void TreeReference::replaceWithAllocationFailure() {
  if (isAllocationFailure()) {
    return;
  }
  assert(isDefined());
  TreeReference p = parent();
  bool hasParent = p.isDefined();
  int indexInParentNode = hasParent ? node()->indexInParent() : -1;
  int currentRetainCount = node()->retainCount();
  TreeNode * staticAllocFailNode = node()->failedAllocationStaticNode(); //TODO was typedNode

  // Move the node to the end of the pool and decrease children count of parent
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), node());
  if (p.isDefined()) {
    p.decrementNumberOfChildren();
  }

  // Release all children and delete the node in the pool
  node()->releaseChildrenAndDestroy();

  /* Create an allocation failure node with the previous node id. We know
   * there is room in the pool as we deleted the previous node and an
   * AllocationFailure nodes size is smaller or equal to any other node size.*/
  //TODO static assert that the size is smaller
  TreeNode * newAllocationFailureNode = TreePool::sharedPool()->deepCopy(staticAllocFailNode);
  newAllocationFailureNode->rename(m_identifier);
  if (p.isDefined()) {
    assert(indexInParentNode >= 0);
    /* Set the refCount to previousRefCount-1 because the previous parent is
     * no longer retaining the node. When we add this node to the parent, it
     * will retain it and increment the retain count. */
    newAllocationFailureNode->setReferenceCounter(currentRetainCount - 1);
    p.addChildTreeAtIndex(TreeRef(newAllocationFailureNode), indexInParentNode);
  } else {
    newAllocationFailureNode->setReferenceCounter(currentRetainCount);
  }
}

void TreeReference::mergeTreeChildrenAtIndex(TreeReference t, int i) {
  assert(i >= 0 && i <= numberOfChildren());
  // Steal operands
  int numberOfNewChildren = t.numberOfChildren();
  if (i < numberOfChildren()) {
    TreePool::sharedPool()->moveChildren(node()->childTreeAtIndex(i), t.node());
  } else {
    TreePool::sharedPool()->moveChildren(node()->lastDescendant()->next(), t.node());
  }
  t.node()->eraseNumberOfChildren();
  // If t is a child, remove it
  if (node()->hasChild(t.node())) {
    removeTreeChild(t);
  }
  node()->incrementNumberOfChildren(numberOfNewChildren);
}

// Private

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
    currentNode->release();
  }
};

}
