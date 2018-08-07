#ifndef POINCARE_TREE_BY_VALUE_H
#define POINCARE_TREE_BY_VALUE_H

#include "tree_by_reference.h"
#include <stdio.h>

namespace Poincare {

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
  int indexOfChild(TreeByValue t) const { return TreeByReference::indexOfChild(t); }

  /* Hierarchy operations */
  // Replace
  TreeByValue replaceChild(TreeByValue oldChild, TreeByValue newChild);
  TreeByValue replaceChildAtIndex(int oldChildIndex, TreeByValue newChild);
  // Merge
  TreeByValue mergeChildrenAtIndex(TreeByValue t, int i);
  // Swap
  TreeByValue swapChildren(int i, int j);

protected:
  /* Constructor */
  TreeByValue(TreeByReference t) : TreeByReference(node()) {}
  TreeByValue(TreeNode * n) : TreeByReference(n) {}

  /* Hierarchy operations */
  // Add
  TreeByValue addChildAtIndex(TreeByValue t, int index, int currentNumberOfChildren);
  // Remove puts a child at the end of the pool
  TreeByValue removeChildAtIndex(int i);
  TreeByValue removeChild(TreeByValue t, int childNumberOfChildren);
  TreeByValue removeChildren(int currentNumberOfChildren);
  TreeByValue removeChildrenAndDestroy(int currentNumberOfChildren);
};

}

#endif
