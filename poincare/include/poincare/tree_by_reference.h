#ifndef POINCARE_TREE_BY_REFERENCE_H
#define POINCARE_TREE_BY_REFERENCE_H

#include <poincare/tree_pool.h>
#include <stdio.h>

namespace Poincare {

class TreeByReference {
  friend class TreeNode;
  friend class TreePool;
public:
  /* Constructors */
  TreeByReference(const TreeByReference & tr) : m_identifier(TreePool::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }
  TreeByReference(TreeByReference&& tr) : m_identifier(TreePool::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }
  ~TreeByReference();

  /* Operators */
  TreeByReference& operator=(const TreeByReference& tr) {
    setTo(tr);
    return *this;
  }
  TreeByReference& operator=(TreeByReference&& tr) {
    setTo(tr);
    return *this;
  }

  /* Comparison */
  inline bool operator==(TreeByReference t) { return m_identifier == t.identifier(); }
  inline bool operator!=(TreeByReference t) { return m_identifier != t.identifier(); }

  /* Clone */
  TreeByReference clone() const;

  int identifier() const { return m_identifier; }
  virtual TreeNode * node() const { return TreePool::sharedPool()->node(m_identifier); }

  bool isUninitialized() const { return node()->isUninitialized(); }
  bool isAllocationFailure() const { return node()->isAllocationFailure(); }
  bool isStatic() const { return node()->isStatic(); }

  int nodeRetainCount() const {
    assert(!isUninitialized());
    return node()->retainCount();
  }
  void incrementNumberOfChildren(int increment = 1) {
    assert(!isUninitialized());
    node()->incrementNumberOfChildren(increment);
  }
  void decrementNumberOfChildren(int decrement = 1) {
    assert(!isUninitialized());
    node()->decrementNumberOfChildren(decrement);
  }
  int numberOfDescendants(bool includeSelf) const {
    assert(!isUninitialized());
    return node()->numberOfDescendants(includeSelf);
  }

  /* Hierarchy */
  bool hasChild(TreeByReference t) const {
    assert(!isUninitialized());
    return node()->hasChild(t.node());
  }
  bool hasSibling(TreeByReference t) const {
    assert(!isUninitialized());
    return node()->hasSibling(t.node());
  }
  bool hasAncestor(TreeByReference t, bool includeSelf) const {
    assert(!isUninitialized());
    return node()->hasAncestor(t.node(), includeSelf);
  }
  int numberOfChildren() const {
    assert(!isUninitialized());
    return node()->numberOfChildren();
  }
  TreeByReference parent() const {
    assert(!isUninitialized());
    return TreeByReference(node()->parent());
  }
  TreeByReference childAtIndex(int i) const {
    assert(!isUninitialized());
    return TreeByReference(node()->childAtIndex(i));
  }
  int indexOfChild(TreeByReference t) const {
    assert(!isUninitialized());
    return node()->indexOfChild(t.node());
  }

  /* Hierarchy operations */
  // Replace
  void replaceWithInPlace(TreeByReference t);
  void replaceChildInPlace(TreeByReference oldChild, TreeByReference newChild);
  void replaceChildAtIndexInPlace(int oldChildIndex, TreeByReference newChild) {
    assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
    TreeByReference oldChild = childAtIndex(oldChildIndex);
    replaceChildInPlace(oldChild, newChild);
  }
  void replaceWithAllocationFailureInPlace(int currentNumberOfChildren);
  void replaceChildWithGhostInPlace(TreeByReference t);
  // Merge
  void mergeChildrenAtIndexInPlace(TreeByReference t, int i);
  // Swap
  void swapChildrenInPlace(int i, int j);

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

protected:
  /* Constructor */
  TreeByReference(const TreeNode * node) {
    assert(node != nullptr);
    setIdentifierAndRetain(node->identifier());
  }
  TreeByReference() : m_identifier(-1) {}
  void setIdentifierAndRetain(int newId) {
    m_identifier = newId;
    node()->retain();
  }
  void setTo(const TreeByReference & tr);
  /* Hierarchy operations */
  // Add
  virtual void addChildAtIndexInPlace(TreeByReference t, int index, int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  virtual void removeChildAtIndexInPlace(int i);
  virtual void removeChildInPlace(TreeByReference t, int childNumberOfChildren);
  virtual void removeChildrenInPlace(int currentNumberOfChildren);

  int m_identifier;

private:
  // Add ghost children on layout construction
  void buildGhostChildren();
};

}

#endif
