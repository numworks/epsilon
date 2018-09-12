#ifndef POINCARE_TREE_BY_REFERENCE_H
#define POINCARE_TREE_BY_REFERENCE_H

#include <poincare/tree_pool.h>
#include <stdio.h>

namespace Poincare {

/* TreeByReference constructors that take only one argument and this argument is
 * a TreeByReference should be marked explicit. This prevents the code from
 * compiling with, for instance: Logarithm l = clone() (which would be
 * equivalent to Logarithm l = Logarithm(clone())). */

class TreeByReference {
  friend class TreeNode;
  friend class TreePool;
public:
  /* Constructors */
  TreeByReference(const TreeByReference & tr) : m_identifier(TreeNode::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  TreeByReference(TreeByReference && tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = TreeNode::NoNodeIdentifier;
  }

  ~TreeByReference() {
    release(m_identifier);
  }

  /* Operators */
  TreeByReference & operator=(const TreeByReference & tr) {
    setTo(tr);
    return *this;
  }

  TreeByReference & operator=(TreeByReference && tr) {
    release(m_identifier);
    m_identifier = tr.m_identifier;
    tr.m_identifier = TreeNode::NoNodeIdentifier;
    return *this;
  }

  /* Comparison */
  inline bool operator==(TreeByReference t) { return m_identifier == t.identifier(); }
  inline bool operator!=(TreeByReference t) { return m_identifier != t.identifier(); }

  /* Clone */
  TreeByReference clone() const;

  int identifier() const { return m_identifier; }
  TreeNode * node() const;
  int nodeRetainCount() const { return node()->retainCount(); }
  size_t size() const;
  void * addressInPool() const { return reinterpret_cast<void *>(node()); }

  bool isGhost() const { return node()->isGhost(); }
  bool isUninitialized() const { return m_identifier == TreeNode::NoNodeIdentifier; }
#if POINCARE_ALLOW_STATIC_NODES
  bool isStatic() const { return node()->isStatic(); }
#endif

  /* Hierarchy */
  bool hasChild(TreeByReference t) const;
  bool hasSibling(TreeByReference t) const { return node()->hasSibling(t.node()); }
  bool hasAncestor(TreeByReference t, bool includeSelf) const { return node()->hasAncestor(t.node(), includeSelf); }
  int numberOfChildren() const { return node()->numberOfChildren(); }
  int indexOfChild(TreeByReference t) const;
  TreeByReference parent() const;
  TreeByReference childAtIndex(int i) const;
  void setParentIdentifier(int id) { node()->setParentIdentifier(id); }
  void deleteParentIdentifier() { node()->deleteParentIdentifier(); }
  void deleteParentIdentifierInChildren() { node()->deleteParentIdentifierInChildren(); }
  void incrementNumberOfChildren(int increment = 1) { node()->incrementNumberOfChildren(increment); }
  void decrementNumberOfChildren(int decrement = 1) { node()->decrementNumberOfChildren(decrement); }
  int numberOfDescendants(bool includeSelf) const { return node()->numberOfDescendants(includeSelf); }

  /* Hierarchy operations */
  // Replace
  void replaceWithInPlace(TreeByReference t);
  void replaceChildInPlace(TreeByReference oldChild, TreeByReference newChild);
  void replaceChildAtIndexInPlace(int oldChildIndex, TreeByReference newChild);
  void replaceChildAtIndexWithGhostInPlace(int index) {
    assert(index >= 0 && index < numberOfChildren());
    replaceChildWithGhostInPlace(childAtIndex(index));
  }
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
  TreeByReference(const TreeNode * node);
  TreeByReference(int nodeIndentifier = TreeNode::NoNodeIdentifier) : m_identifier(nodeIndentifier) {}
  void setIdentifierAndRetain(int newId);
  void setTo(const TreeByReference & tr);

  /* Hierarchy operations */
  // Add
  void addChildAtIndexInPlace(TreeByReference t, int index, int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  void removeChildAtIndexInPlace(int i);
  void removeChildInPlace(TreeByReference t, int childNumberOfChildren);
  void removeChildrenInPlace(int currentNumberOfChildren);

  int m_identifier;

private:
  void detachFromParent();
  // Add ghost children on layout construction
  void buildGhostChildren();
  void release(int identifier);
};

}

#endif
