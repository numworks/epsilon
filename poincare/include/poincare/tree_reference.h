#ifndef POINCARE_TREE_REFERENCE_H
#define POINCARE_TREE_REFERENCE_H

#include "tree_pool.h"
#include <stdio.h>

namespace Poincare {

template <typename T>
class TreeReference {
  friend class TreeNode;
  friend class AdditionNode;
  friend class ExpressionNode;
  template <typename U>
  friend class TreeReference;
  template <typename U>
  friend class ExpressionReference;
  template <typename U>
  friend class LayoutReference;
public:
  TreeReference(const TreeReference & tr) { setTo(tr); }
  TreeReference(TreeReference&& tr) { setTo(tr); }
  TreeReference& operator=(const TreeReference& tr) {
    setTo(tr);
    return *this;
  }
  TreeReference& operator=(TreeReference&& tr) {
    setTo(tr);
    return *this;
  }

  inline bool operator==(TreeReference<TreeNode> t) { return m_identifier == t.identifier(); }

  TreeReference<T> clone() const {
    TreeNode * myNode = node();
    if (myNode->isAllocationFailure()) {
      int allocationFailureNodeId = myNode->allocationFailureNodeIdentifier();
      return TreeReference<T>(TreePool::sharedPool()->node(allocationFailureNodeId));
    }
    TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(myNode);
    nodeCopy->deepResetReferenceCounter();
    return TreeReference<T>(nodeCopy);
  }

  ~TreeReference() {
    if (m_identifier >= 0) {
      assert(node());
      assert(node()->identifier() == m_identifier);
      node()->release();
    }
  }
  operator TreeReference<TreeNode>() const { return TreeReference<TreeNode>(this->node()); }

  int identifier() const { return m_identifier; }
  TreeNode * node() const { return TreePool::sharedPool()->node(m_identifier); }
  T * typedNode() const {
    // TODO: Here, assert that the node type is indeed T
    // ?? Might be allocation failure, not T
    return static_cast<T*>(node());
  }

  bool isDefined() const { return m_identifier >= 0 && node() != nullptr; } //TODO m_identifier != -1
  bool isAllocationFailure() const { return node()->isAllocationFailure(); }

  int nodeRetainCount() const { return node()->retainCount(); }
  void incrementNumberOfChildren(int increment = 1) { return node()->incrementNumberOfChildren(increment); }
  void decrementNumberOfChildren(int decrement = 1) { return node()->decrementNumberOfChildren(decrement); }

  // Serialization
  bool needsParenthesisWithParent(TreeReference<TreeNode> parentRef) { return node()->needsParenthesisWithParent(parentRef.node()); }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const {
    return node()->writeTextInBuffer(buffer, bufferSize, numberOfSignificantDigits);
  }

  // Hierarchy
  bool hasChild(TreeReference<TreeNode> t) const { return node()->hasChild(t.node()); };
  int numberOfChildren() const { return node()->numberOfChildren(); }
  TreeReference<T> parent() const { return TreeReference(node()->parentTree()); }
  TreeReference<T> treeChildAtIndex(int i) const { return TreeReference(node()->childTreeAtIndex(i)); }

  // Hierarchy operations

  void addChild(TreeReference<TreeNode> t) { return addChildAtIndex(t, 0); }

  void addChildAtIndex(TreeReference<TreeNode> t, int index) {
    if (node()->isAllocationFailure()) {
      return;
    }
    assert(index >= 0 && index <= numberOfChildren());

    // Retain t before detaching it, else it might get destroyed
    t.node()->retain();

    // Detach t from its parent
    TreeReference<TreeNode> tParent = t.parent();
    if (tParent.isDefined()) {
      tParent.removeChild(t);
    }

    // Move t
    TreeNode * newChildPosition = node()->next();
    for (int i = 0; i < index; i++) {
      newChildPosition = newChildPosition->nextSibling();
    }
    TreePool::sharedPool()->move(t.node(), newChildPosition);
    node()->incrementNumberOfChildren();
  }

  void removeChild(TreeReference<TreeNode> t) {
    TreePool::sharedPool()->move(t.node(), TreePool::sharedPool()->last());
    t.node()->release();
    node()->decrementNumberOfChildren();
  }

  void removeChildren() {
    node()->releaseChildren();
    TreePool::sharedPool()->moveChildren(node(), TreePool::sharedPool()->last());
    node()->eraseNumberOfChildren();
  }

  void replaceWith(TreeReference<TreeNode> t) {
    TreeReference<TreeNode> p = parent();
    if (p.isDefined()) {
      p.replaceChildAtIndex(p.node()->indexOfChildByIdentifier(identifier()), t);
    }
  }

  void replaceChildAtIndex(int oldChildIndex, TreeReference<TreeNode> newChild) {
    if (newChild.isAllocationFailure()) {
      replaceWithAllocationFailure();
      return;
    }
    TreeReference<TreeNode> p = newChild.parent();
    if (p.isDefined()) {
      p.decrementNumberOfChildren();
    }
    assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
    TreeReference<T> oldChild = treeChildAtIndex(oldChildIndex);
    TreePool::sharedPool()->move(newChild.node(), oldChild.node()->next());
    newChild.node()->retain();
    TreePool::sharedPool()->move(oldChild.node(), TreePool::sharedPool()->last());
    oldChild.node()->release();
  }

  void replaceWithAllocationFailure() {
    TreeReference<TreeNode> p = parent();
    bool hasParent = p.isDefined();
    int indexInParentNode = hasParent ? node()->indexInParent() : -1;
    int currentRetainCount = node()->retainCount();
    TreeNode * staticAllocFailNode = typedNode()->failedAllocationStaticNode();

    // Move the node to the end of the pool and decrease children count of parent
    TreePool::sharedPool()->move(node(), TreePool::sharedPool()->last());
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
      p.addChildAtIndex(newAllocationFailureNode, indexInParentNode);
    } else {
      newAllocationFailureNode->setReferenceCounter(currentRetainCount);
    }
  }

  void swapChildren(int i, int j) {
    assert(i >= 0 && i < numberOfChildren());
    assert(j >= 0 && j < numberOfChildren());
    if (i == j) {
      return;
    }
    int firstChildIndex = i < j ? i : j;
    int secondChildIndex = i > j ? i : j;
    TreeReference<T> firstChild = treeChildAtIndex(firstChildIndex);
    TreeReference<T> secondChild = treeChildAtIndex(secondChildIndex);
    TreeNode * firstChildNode = firstChild.node();
    TreePool::sharedPool()->move(firstChildNode, secondChild.node()->next());
    TreePool::sharedPool()->move(secondChild.node(), firstChildNode);
  }

  void mergeChildren(TreeReference<T> t) {
    // Steal operands
    int numberOfNewChildren = t.numberOfChildren();
    TreePool::sharedPool()->moveChildren(t.node(), node()->lastDescendant()->next());
    t.node()->eraseNumberOfChildren();
    // If t is a child, remove it
    if (node()->hasChild(t.node())) {
      removeChild(t);
    }
    node()->incrementNumberOfChildren(numberOfNewChildren);
  }

  TreeReference(TreeNode * node) { // TODO Make this protected
    if (node == nullptr) {
      m_identifier = -1;
    } else {
      setIdentifierAndRetain(node->identifier());
    }
  }

protected:
  TreeReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<T>();
    m_identifier = node->identifier();
  }

  void setIdentifierAndRetain(int newId) {
    m_identifier = newId;
    node()->retain();
  }
private:
  void setTo(const TreeReference & tr) {
    setIdentifierAndRetain(tr.identifier());
  }
  int m_identifier;
};

typedef TreeReference<TreeNode> TreeRef;

}

#endif
