#ifndef POINCARE_TREE_BY_REFERENCE_H
#define POINCARE_TREE_BY_REFERENCE_H

#include "tree_pool.h"
#include <stdio.h>

namespace Poincare {

class TreeByReference {
  friend class TreeNode;
public:
  /* Constructors */
  TreeByReference(const TreeByReference & tr) : m_identifier(TreePool::NoNodeIdentifier) { setTo(tr); }
  TreeByReference(TreeByReference&& tr) : m_identifier(TreePool::NoNodeIdentifier) { setTo(tr); }
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

  bool isDefined() const { return m_identifier != TreePool::NoNodeIdentifier && node() != nullptr; }
  bool isAllocationFailure() const { return isDefined() && node()->isAllocationFailure(); }

  int nodeRetainCount() const {
    assert(isDefined());
    return node()->retainCount();
  }
  void incrementNumberOfChildren(int increment = 1) {
    assert(isDefined());
    node()->incrementNumberOfChildren(increment);
  }
  void decrementNumberOfChildren(int decrement = 1) {
    assert(isDefined());
    node()->decrementNumberOfChildren(decrement);
  }
  int numberOfDescendants(bool includeSelf) const {
    assert(isDefined());
    return node()->numberOfDescendants(includeSelf);
  }

  /* Hierarchy */
  bool hasChild(TreeByReference t) const {
    assert(isDefined());
    return node()->hasChild(t.node());
  }
  bool hasSibling(TreeByReference t) const {
    assert(isDefined());
    return node()->hasSibling(t.node());
  }
  bool hasAncestor(TreeByReference t, bool includeSelf) const {
    assert(isDefined());
    return node()->hasAncestor(t.node(), includeSelf);
  }
  int numberOfChildren() const {
    assert(isDefined());
    return node()->numberOfChildren();
  }
  TreeByReference parent() const {
    assert(isDefined());
    return TreeByReference(node()->parent());
  }
  TreeByReference treeChildAtIndex(int i) const {
    assert(isDefined());
    return TreeByReference(node()->childAtIndex(i));
  }
  int indexOfChild(TreeByReference t) const {
    assert(isDefined());
    return node()->indexOfChild(t.node());
  }

  /* Hierarchy operations */
  // Replace
  void replaceWith(TreeByReference t);
  void replaceTreeChild(TreeByReference oldChild, TreeByReference newChild);
  void replaceTreeChildAtIndex(int oldChildIndex, TreeByReference newChild) {
    assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
    TreeByReference oldChild = treeChildAtIndex(oldChildIndex);
    replaceTreeChild(oldChild, newChild);
  }
  void replaceWithAllocationFailure(int currentNumberOfChildren);
  void replaceChildWithGhost(TreeByReference t);
  // Merge
  void mergeTreeChildrenAtIndex(TreeByReference t, int i);
  // Swap
  void swapChildren(int i, int j);

protected:
  /* Constructor */
  TreeByReference(const TreeNode * node, bool isCreatingNode = false) {
    if (node == nullptr) {
      m_identifier = TreePool::NoNodeIdentifier;
      return;
    }
    setIdentifierAndRetain(node->identifier());
    if (isCreatingNode) {
      buildGhostChildren();
    }
  }

  /* Hierarchy operations */
  // Add
  virtual void addChildTreeAtIndex(TreeByReference t, int index, int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  virtual void removeChildTreeAtIndex(int i);
  virtual void removeChildTree(TreeByReference t, int childNumberOfChildren);
  virtual void removeChildren(int currentNumberOfChildren);
  virtual void removeChildrenAndDestroy(int currentNumberOfChildren);

  int m_identifier;
private:
  TreeByReference() : m_identifier(-1) {}
  void setTo(const TreeByReference & tr);

  // Add ghost children on layout construction
  void buildGhostChildren();

  void setIdentifierAndRetain(int newId) {
    m_identifier = newId;
    node()->retain();
  }
};

}

#endif
