#ifndef POINCARE_TREE_BY_VALUE_H
#define POINCARE_TREE_BY_VALUE_H

#include "tree_by_reference.h"
#include <stdio.h>

namespace Poincare {

/* When looking for information about a child, it must be passed by reference,
 * else it is copied so it is no longer a child. This is important for instance
 * in indexOfChild and in replaceChild. */

class TreeByValue : virtual protected TreeByReference {
public:
  /* Constructors */
  TreeByValue(const TreeByValue & tr);
  /* Operators */
  TreeByValue& operator=(const TreeByValue& tr);

  bool isDefined() const { return TreeByReference::isDefined(); }

  /* Hierarchy */
  int numberOfChildren() const { return TreeByReference::numberOfChildren(); }
  TreeByValue parent() const { return TreeByValue(TreeByReference::parent()); }
  TreeByValue treeChildAtIndex(int i) const { return TreeByValue(TreeByReference::treeChildAtIndex(i)); }
  int indexOfChild(TreeByReference t) const { return TreeByReference::indexOfChild(t); }

  /* Hierarchy operations */
  // Replace
  void replaceChildInPlace(TreeByReference oldChild, TreeByValue newChild) {
    TreeByReference::replaceChildInPlace(oldChild, newChild);
  }
  void replaceChildAtIndexInPlace(int oldChildIndex, TreeByValue newChild) {
    TreeByReference::replaceChildAtIndexInPlace(oldChildIndex, newChild);
  }
  // Merge
  void mergeChildrenAtIndexInPlace(TreeByValue t, int i) {
    TreeByReference::mergeChildrenAtIndexInPlace(t, i);
  }
  // Swap
  void swapChildrenInPlace(int i, int j) {
    TreeByReference::swapChildrenInPlace(i, j);
  }

protected:
  /* Constructor */
  TreeByValue(TreeByReference t) : TreeByReference(node()) {}
  TreeByValue(const TreeNode * n) : TreeByReference(n) {}

  /* Hierarchy operations */
  // Add
  void addChildAtIndexInPlace(TreeByValue t, int index, int currentNumberOfChildren) {
    TreeByReference::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
  }
  // Remove puts a child at the end of the pool
  void removeChildInPlace(TreeByValue t, int childNumberOfChildren) {
    TreeByReference::removeChildInPlace(t, childNumberOfChildren);
  }
};

}

#endif
