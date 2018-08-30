#ifndef POINCARE_TREE_BY_VALUE_H
#define POINCARE_TREE_BY_VALUE_H

#include "tree_by_reference.h"
#include <stdio.h>

namespace Poincare {

/* When looking for information about a child, it must be passed by reference,
 * else it is copied so it is no longer a child. This is important for instance
 * in indexOfChild and in replaceChild. */

class TreeByValue : protected TreeByReference {
public:
  /* Constructors */
  TreeByValue(const TreeByValue & tr);
  /* Operators */
  TreeByValue& operator=(const TreeByValue& tr);

  virtual ~TreeByValue() = default;

  bool isUninitialized() const { return TreeByReference::isUninitialized(); }
  bool isAllocationFailure() const { return TreeByReference::isAllocationFailure(); }

  TreeNode * node() const { return TreeByReference::node(); }

  /* Hierarchy */
  int numberOfChildren() const { return TreeByReference::numberOfChildren(); }
  TreeByValue parent() const { return TreeByValue(TreeByReference::parent()); }
  TreeByValue childAtIndex(int i) const { return TreeByValue(TreeByReference::childAtIndex(i)); }
  int indexOfChild(TreeByReference t) const { return TreeByReference::indexOfChild(t); }

  /* Hierarchy operations */
  // Replace
  void replaceChildInPlace(TreeByReference oldChild, TreeByValue newChild) {
    TreeByReference::replaceChildInPlace(oldChild, newChild);
  }
  void replaceChildAtIndexInPlace(int oldChildIndex, TreeByValue newChild) {
    TreeByReference::replaceChildAtIndexInPlace(oldChildIndex, newChild);
  }
  void replaceWithInPlace(TreeByReference t) {
    TreeByReference::replaceWithInPlace(t);
  }

  // Swap
  void swapChildrenInPlace(int i, int j) {
    TreeByReference::swapChildrenInPlace(i, j);
  }
  using TreeByReference::identifier;
#if POINCARE_TREE_LOG
  using TreeByReference::log;
#endif

protected:
  /* Constructor */
  TreeByValue(TreeByReference t) : TreeByReference(t.node()) {}
  TreeByValue(const TreeNode * n) : TreeByReference(n) {}

  /* Hierarchy operations */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
  void addChildAtIndexInPlace(TreeByValue t, int index, int currentNumberOfChildren) {
    TreeByReference::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
  }
#pragma clang diagnostic pop
};

}

#endif
