#ifndef POINCARE_FLOAT_LIST_H
#define POINCARE_FLOAT_LIST_H

#include <poincare/expression.h>
#include <poincare/statistics/statistics.h>

namespace Poincare {

template <typename T>
class FloatList : public Expression, public DatasetColumn<T> {
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

#endif
