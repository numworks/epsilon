#include <poincare/tree_by_value.h>

namespace Poincare {

/* Constructors */

TreeByValue::TreeByValue(const TreeByValue & tr) :
  TreeByValue(clone()) {}

/* Operators */
TreeByValue& TreeByValue::operator=(const TreeByValue& tr) {
  TreeByReference t = clone();
  m_identifier = t.identifier();
  return *this;
}

/* Hierarchy operations */

TreeByValue TreeByValue::replaceChild(TreeByValue oldChild, TreeByValue newChild) {
  TreeByValue t = *this;
  t.replaceChildInPlace(oldChild, newChild);
  return t;
}

TreeByValue TreeByValue::replaceChildAtIndex(int oldChildIndex, TreeByValue newChild) {
  TreeByValue t = *this;
  t.replaceChildAtIndexInPlace(oldChildIndex, newChild);
  return t;
}

TreeByValue TreeByValue::mergeChildrenAtIndex(TreeByValue child, int i) {
  TreeByValue t = *this;
  t.mergeChildrenAtIndexInPlace(child, i);
  return t;
}

TreeByValue TreeByValue::swapChildren(int i, int j) {
  TreeByValue t = *this;
  t.swapChildrenInPlace(i, j);
  return t;
}

/* Protected */

// Add

TreeByValue TreeByValue::addChildAtIndex(TreeByValue u, int index, int currentNumberOfChildren) {
  TreeByValue t = *this;
  t.addChildAtIndexInPlace(u, index, currentNumberOfChildren);
  return t;
}

// Remove

TreeByValue TreeByValue::removeChildAtIndex(int i) {
  TreeByValue t = *this;
  t.removeChildAtIndexInPlace(i);
  return t;
}

TreeByValue TreeByValue::removeChild(TreeByValue u, int childNumberOfChildren) {
  TreeByValue t = *this;
  t.removeChildInPlace(u, childNumberOfChildren);
  return t;
}

TreeByValue TreeByValue::removeChildren(int currentNumberOfChildren) {
  TreeByValue t = *this;
  t.removeChildrenInPlace(currentNumberOfChildren);
  return t;
}

TreeByValue TreeByValue::removeChildrenAndDestroy(int currentNumberOfChildren) {
  TreeByValue t = *this;
  t.removeChildrenAndDestroyInPlace(currentNumberOfChildren);
  return t;
}

}
