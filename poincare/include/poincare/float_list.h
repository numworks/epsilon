#ifndef POINCARE_FLOAT_LIST_H
#define POINCARE_FLOAT_LIST_H

#include "list.h"
#include "dataset_column.h"
#include "float.h"

namespace Poincare {

template<typename T>
class FloatList : public List, public DatasetColumn<T> {

/* WARNING: Do not add children to FloatList with addChildAtIndex,
 * and do not replace children with replaceChildAtIndex.
 * The method floatExpressionAtIndex assumes that every child is a FloatNode.
 * Use addValueAtIndex and replaceValueAtIndex instead.
 */
public:
  static FloatList<T> Builder() { return TreeHandle::NAryBuilder<FloatList<T>, ListNode>(); }

  void addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren) = delete;
  void replaceChildInPlace(TreeHandle oldChild, TreeHandle newChild) = delete;

  void addValueAtIndex(T value, int index);
  void replaceValueAtIndex(T value, int index);
  void removeValueAtIndex(int index);
  T valueAtIndex(int index) const override;
  int length() const override { return isUninitialized() ? 0: numberOfChildren(); }

  /* This replaces childAtIndex. Instead of being in linear time, it's
   * in constant time. */
  Expression floatExpressionAtIndex(int index) const;
};

}


#endif
