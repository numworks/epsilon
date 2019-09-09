#ifndef POINCARE_TREE_BY_REFERENCE_H
#define POINCARE_TREE_BY_REFERENCE_H

#include <poincare/tree_pool.h>
#include <stdio.h>

namespace Shared {
  class ContinuousFunction;
}

namespace Poincare {
/* TODO: implement an iterator over the children, so we can use "for (TreeHandle
 * c : children)" instead of a for loop over the child index. This should be
 * faster, as we do not have to recompute childAtIndex(i) at each iteration.
 * Caution:
 *  - The iterator should be specialized for Expression, Layout...
 *  - We cannot compose with a TreeNode iterator as the node pointers might
 *  change during the loop. */

/* TreeHandle constructors that take only one argument and this argument is
 * a TreeHandle should be marked explicit. This prevents the code from
 * compiling with, for instance: Logarithm l = clone() (which would be
 * equivalent to Logarithm l = Logarithm(clone())). */

class TreeHandle {
  template<class T>
  friend class ArrayBuilder;
  friend class ::Shared::ContinuousFunction;
  friend class TreeNode;
  friend class TreePool;
public:
  /* Constructors */
  TreeHandle(const TreeHandle & tr) : m_identifier(TreeNode::NoNodeIdentifier) {
    setIdentifierAndRetain(tr.identifier());
  }

  TreeHandle(TreeHandle && tr) : m_identifier(tr.m_identifier) {
    tr.m_identifier = TreeNode::NoNodeIdentifier;
  }

  ~TreeHandle() {
    release(m_identifier);
  }

  /* Operators */
  TreeHandle & operator=(const TreeHandle & tr) {
    setTo(tr);
    return *this;
  }

  TreeHandle & operator=(TreeHandle && tr) {
    release(m_identifier);
    m_identifier = tr.m_identifier;
    tr.m_identifier = TreeNode::NoNodeIdentifier;
    return *this;
  }

  /* Comparison */
  inline bool operator==(const TreeHandle& t) { return m_identifier == t.identifier(); }
  inline bool operator!=(const TreeHandle& t) { return m_identifier != t.identifier(); }

  /* Clone */
  TreeHandle clone() const;

  int identifier() const { return m_identifier; }
  TreeNode * node() const;
  int nodeRetainCount() const { return node()->retainCount(); }
  size_t size() const;
  void * addressInPool() const { return reinterpret_cast<void *>(node()); }

  bool isGhost() const { return node()->isGhost(); }
  bool isUninitialized() const { return m_identifier == TreeNode::NoNodeIdentifier; }

  /* Hierarchy */
  bool hasChild(TreeHandle t) const;
  bool hasSibling(TreeHandle t) const { return node()->hasSibling(t.node()); }
  bool hasAncestor(TreeHandle t, bool includeSelf) const { return node()->hasAncestor(t.node(), includeSelf); }
  int numberOfChildren() const { return node()->numberOfChildren(); }
  int indexOfChild(TreeHandle t) const;
  TreeHandle parent() const;
  TreeHandle childAtIndex(int i) const;
  void setParentIdentifier(int id) { node()->setParentIdentifier(id); }
  void deleteParentIdentifier() { node()->deleteParentIdentifier(); }
  void deleteParentIdentifierInChildren() { node()->deleteParentIdentifierInChildren(); }
  void incrementNumberOfChildren(int increment = 1) { node()->incrementNumberOfChildren(increment); }
  void decrementNumberOfChildren(int decrement = 1) { node()->decrementNumberOfChildren(decrement); }
  int numberOfDescendants(bool includeSelf) const { return node()->numberOfDescendants(includeSelf); }

  /* Hierarchy operations */
  // Replace
  void replaceWithInPlace(TreeHandle t);
  void replaceChildInPlace(TreeHandle oldChild, TreeHandle newChild);
  void replaceChildAtIndexInPlace(int oldChildIndex, TreeHandle newChild);
  void replaceChildAtIndexWithGhostInPlace(int index) {
    assert(index >= 0 && index < numberOfChildren());
    replaceChildWithGhostInPlace(childAtIndex(index));
  }
  void replaceChildWithGhostInPlace(TreeHandle t);
  // Merge
  void mergeChildrenAtIndexInPlace(TreeHandle t, int i);
  // Swap
  void swapChildrenInPlace(int i, int j);

  /* Logging */
#if POINCARE_TREE_LOG
  void log() const;
#endif

protected:
  /* Constructor */
  TreeHandle(const TreeNode * node);
  // Un-inlining this constructor actually inscreases the firmware size
  TreeHandle(int nodeIndentifier = TreeNode::NoNodeIdentifier) : m_identifier(nodeIndentifier) {
    if (hasNode(nodeIndentifier)) {
      node()->retain();
    }
  }

  // WARNING: if the children table is the result of a cast, the object downcasted has to be the same size as a TreeHandle.
  template <class T, class U>
  static T NAryBuilder(TreeHandle * children = nullptr, size_t numberOfChildren = 0);
  template <class T, class U>
  static T FixedArityBuilder(TreeHandle * children = nullptr, size_t numberOfChildren = 0);

  static TreeHandle BuildWithGhostChildren(TreeNode * node);

  void setIdentifierAndRetain(int newId);
  void setTo(const TreeHandle & tr);

  static bool hasNode(int identifier) { return identifier > TreeNode::NoNodeIdentifier; }

  /* Hierarchy operations */
  // Add
  void addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  void removeChildAtIndexInPlace(int i);
  void removeChildInPlace(TreeHandle t, int childNumberOfChildren);
  void removeChildrenInPlace(int currentNumberOfChildren);

  int m_identifier;

private:
  template <class U>
  static TreeHandle Builder();

  void detachFromParent();
  // Add ghost children on layout construction
  void buildGhostChildren();
  void release(int identifier);
};

}

#endif
