#ifndef TREE_REFERENCE_H
#define TREE_REFERENCE_H

#include "tree_pool.h"
#include <stdio.h>

static inline int min(int i, int j) { return i < j ? i : j; }
static inline int max(int i, int j) { return i > j ? i : j; }

class Cursor;

template <typename T>
class TreeReference {
  friend class Cursor;
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

  void setTo(const TreeReference & tr) {
    setIdentifierAndRetain(tr.identifier());
  }

  TreeReference<T> clone() {
    TreeNode * myNode = node();
    if (myNode->isAllocationFailure()) {
      return TreeReference<T>(TreePool::sharedPool()->node(TreePool::AllocationFailureIdentifier));
    }
    TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(myNode);
    return TreeReference<T>(nodeCopy);
  }

  ~TreeReference() {
    if (m_identifier >= 0) {
      assert(node());
      assert(node()->identifier() == m_identifier);
      node()->release();
    }
  }

  bool isDefined() { return m_identifier >= 0 && node() != nullptr; }
  bool isAllocationFailure() { return node()->isAllocationFailure(); }

  int nodeRetainCount() { return node()->retainCount(); }
  void incrementNumberOfChildren() { return node()->incrementNumberOfChildren(); }
  void decrementNumberOfChildren() { return node()->decrementNumberOfChildren(); }

  operator TreeReference<TreeNode>() {
    return TreeReference<TreeNode>(this->node());
  }

  T * castedNode() {
    // TODO: Here, assert that the node type is indeed T
    // ?? Might be allocation failure, not T
    updateIdentifier();
    return static_cast<T*>(TreePool::sharedPool()->node(m_identifier));
  }

  TreeNode * node() {
    updateIdentifier();
    return TreePool::sharedPool()->node(m_identifier);
  }

  void updateIdentifier() {
    /* If the node was replaced with an allocation failure, change the
     * identifier */
    if (TreePool::sharedPool()->nodeWasReplacedWithAllocationFailure(m_identifier)) {
      TreePool::sharedPool()->releaseAllocationFailure(m_identifier);
      setIdentifierAndRetain(TreePool::AllocationFailureIdentifier);
    }
  }

  int identifier() const { return m_identifier; }

  // Hierarchy
  int numberOfChildren() {
    return node()->numberOfChildren();
  }

  TreeReference<T> parent() {
    return TreeReference(node()->parentTree());
  }

  TreeReference<T> treeChildAtIndex(int i) {
    return TreeReference(node()->childTreeAtIndex(i));
  }

  // Hierarchy operations

  void addTreeChild(TreeReference<TreeNode> t) {
    t.node()->retain();
    TreePool::sharedPool()->move(t.node(), node()->next());
    node()->incrementNumberOfChildren();
  }

  void removeChild(TreeReference<TreeNode> t) {
    TreePool::sharedPool()->move(t.node(), TreePool::sharedPool()->last());
    t.node()->release();
  }

  void replaceWith(TreeReference<TreeNode> t) {
    TreeReference<TreeNode> p = parent();
    p.replaceChildAtIndex(p.node()->indexOfChildByIdentifier(identifier()), t);
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
    int currentRetainCount = node()->retainCount();

    // Move the node to the end of the pool and decrease children count of parent
    TreePool::sharedPool()->move(node(), TreePool::sharedPool()->last());
    if (p.isDefined()) {
      p.decrementNumberOfChildren();
    }

    // Release all children and delete the node in the pool
    node()->releaseChildrenAndDestroy();

    /* In case another ref is pointing to the node, register the identifier as
     * AllocationFailure. If the retainCount is 1 (this reference is the only
     * one pointing to the node), the pool will register the identifier as
     * free. */
    TreePool::sharedPool()->registerIdentiferAsAllocationFailure(m_identifier, currentRetainCount - 1);
    setIdentifierAndRetain(TreePool::AllocationFailureIdentifier);

    // Replace parent with AllocationFailure
    if (p.isDefined()) {
      p.replaceWithAllocationFailure();
    }
  }

  void swapChildren(int i, int j) {
    assert(i >= 0 && i < numberOfChildren());
    assert(j >= 0 && j < numberOfChildren());
    if (i == j) {
      return;
    }
    int firstChildIndex = min(i, j);
    int secondChildIndex = max(i, j);
    TreeReference<T> firstChild = treeChildAtIndex(firstChildIndex);
    TreeReference<T> secondChild = treeChildAtIndex(secondChildIndex);
    TreeNode * firstChildNode = firstChild.node();
    TreePool::sharedPool()->move(firstChildNode, secondChild.node()->next());
    TreePool::sharedPool()->move(secondChild.node(), firstChildNode);
  }

protected:
  TreeReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<T>();
    m_identifier = node->identifier();
  }

  TreeReference(TreeNode * node) {
    if (node == nullptr) {
      m_identifier = -1;
    } else {
      setIdentifierAndRetain(node->identifier());
    }
  }
  void setIdentifierAndRetain(int newId) {
    m_identifier = newId;
    node()->retain();
  }
private:
  int m_identifier;
};

typedef TreeReference<TreeNode> TreeRef;

#endif
