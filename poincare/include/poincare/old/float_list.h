#ifndef POINCARE_FLOAT_LIST_H
#define POINCARE_FLOAT_LIST_H

#include <poincare/expression.h>
#include <poincare/statistics/statistics_dataset.h>

namespace Poincare {

template <typename T>
class FloatList : public List, public DatasetColumn<T> {
 public:
  static FloatList<T> Builder() {
    List list = List::Builder();
    return static_cast<FloatList<T>&>(list);
  }

  void addValueAtIndex(T value, int index);
  void replaceValueAtIndex(T value, int index);
  void removeValueAtIndex(int index);
  T valueAtIndex(int index) const override;
  int length() const override {
    return isUninitialized() ? 0 : numberOfChildren();
  }
};

}  // namespace Poincare

#endif
