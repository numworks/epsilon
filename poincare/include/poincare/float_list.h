#ifndef POINCARE_FLOAT_LIST_H
#define POINCARE_FLOAT_LIST_H

#include "list.h"
#include "dataset_column.h"
#include "float.h"

namespace Poincare {

template<typename T>
class FloatList : public List, public DatasetColumn<T> {

public:
  static FloatList<T> Builder() { return TreeHandle::NAryBuilder<FloatList<T>, ListNode>(); }

  void addValueAtIndex(T value, int index) {
    assert(index <= numberOfChildren());
    List::addChildAtIndexInPlace(Float<T>::Builder(value), index, numberOfChildren());
  }

  void replaceValueAtIndex(T value, int index) {
    assert(index < numberOfChildren());
    List::replaceChildAtIndexInPlace(index, Float<T>::Builder(value));
  }

  void removeValueAtIndex(int index) {
    assert(index < numberOfChildren());
    List::removeChildAtIndexInPlace(index);
  }

  T valueAtIndex(int index) const override {
    if (index >= numberOfChildren()) {
      return NAN;
    }
    Expression child = childAtIndex(index);
    assert((child.type() == ExpressionNode::Type::Float && sizeof(T) == sizeof(float)) || (child.type() == ExpressionNode::Type::Double && sizeof(T) == sizeof(double)));
    return static_cast<Float<T> &>(child).value();
  }

  int length() const override { return numberOfChildren(); }

};

}


#endif
