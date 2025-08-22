#pragma once

#include <poincare/expression.h>
#include <poincare/statistics/dataset_column.h>

namespace Poincare {

template <typename T>
class FloatList : public SystemExpression, public DatasetColumn<T> {
 public:
  static FloatList<T> Builder();

  void addValue(T value);
  void replaceValueAtIndex(T value, int index);
  void removeValueAtIndex(int index);
  T valueAtIndex(int index) const override;
  int length() const override {
    return isUninitialized() ? 0 : numberOfChildren();
  }
};

}  // namespace Poincare
