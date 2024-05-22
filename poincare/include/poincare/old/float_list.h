#ifndef POINCARE_FLOAT_LIST_H
#define POINCARE_FLOAT_LIST_H

#include <poincare/numeric/statistics.h>

#include "junior_expression.h"

namespace Poincare {

template <typename T>
class FloatList : public List, public DatasetColumn<T> {
  /* WARNING: Do not add children to FloatList with addChildAtIndexInPlace,
   * and do not replace children with replaceChildAtIndexInPlace.
   * The method floatExpressionAtIndex assumes that every child is a FloatNode,
   * to optimize computation time.
   * addChildAtIndexInPlace could add a child which is not a FloatNode and
   * lead to bad access when using floatExpressionAtIndex.
   * Use addValueAtIndex and replaceValueAtIndex instead.
   */
 public:
  static FloatList<T> Builder() {
    List list = List::Builder();
    return static_cast<FloatList<T>&>(list);
  }

  void addChildAtIndexInPlace(PoolHandle t, int index,
                              int currentNumberOfChildren) = delete;
  void replaceChildInPlace(PoolHandle oldChild, PoolHandle newChild) = delete;

  void addValueAtIndex(T value, int index);
  void replaceValueAtIndex(T value, int index);
  void removeValueAtIndex(int index);
  T valueAtIndex(int index) const override;
  int length() const override {
    return isUninitialized() ? 0 : numberOfChildren();
  }

  /* This replaces childAtIndex. Instead of being in linear time, it's
   * in constant time. */
  Internal::Tree* floatNodeAtIndex(int index) const;
};

}  // namespace Poincare

#endif
