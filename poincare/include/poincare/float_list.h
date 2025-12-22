#pragma once

#include <poincare/statistics/dataset_column.h>
#include <poincare/system_expression.h>

namespace Poincare {

template <typename T>
class FloatList : public SystemExpression, public DatasetColumn<T> {
 public:
  static FloatList<T> Builder();

  FloatList cloneAndAddValue(T value) const;
  FloatList cloneAndReplaceValueAtIndex(T value, int index) const;
  FloatList cloneAndRemoveValueAtIndex(int index) const;

  T valueAtIndex(int index) const override;
  int length() const override {
    return isUninitialized() ? 0 : numberOfChildren();
  }
};

}  // namespace Poincare
