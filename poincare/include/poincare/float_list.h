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
    Expression child = childAtIndexWithChildrenOfSameSize(index);
    assert((child.type() == ExpressionNode::Type::Float && sizeof(T) == sizeof(float)) || (child.type() == ExpressionNode::Type::Double && sizeof(T) == sizeof(double)));
    static_cast<Float<T> &>(child).setValue(value);
  }

  void removeValueAtIndex(int index) {
    assert(index < numberOfChildren());
    List::removeChildAtIndexInPlace(index);
  }

  T valueAtIndex(int index) const override {
    if (index >= numberOfChildren()) {
      return NAN;
    }
    Expression child = childAtIndexWithChildrenOfSameSize(index);
    assert((child.type() == ExpressionNode::Type::Float && sizeof(T) == sizeof(float)) || (child.type() == ExpressionNode::Type::Double && sizeof(T) == sizeof(double)));
    return static_cast<Float<T> &>(child).value();
  }

  int length() const override { return numberOfChildren(); }

  /* This replaces childAtIndex. Instead of being in linear time, it's
   * in constant time. */
  Expression childAtIndexWithChildrenOfSameSize(int index) const {
    assert(index < numberOfChildren() && numberOfChildren() > 0);
    ExpressionNode * myNode = node();
    char * firstChild = reinterpret_cast<char *>(myNode) + Helpers::AlignedSize(myNode->size(), ByteAlignment);
    return Expression(reinterpret_cast<ExpressionNode *>(firstChild + index * childAtIndex(0).size()));
  }

};

}


#endif
