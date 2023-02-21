#include <poincare/float_list.h>

namespace Poincare {

template <typename T>
void FloatList<T>::addValueAtIndex(T value, int index) {
  assert(index <= numberOfChildren());
  List::addChildAtIndexInPlace(Float<T>::Builder(value), index,
                               numberOfChildren());
}

template <typename T>
void FloatList<T>::replaceValueAtIndex(T value, int index) {
  assert(index < numberOfChildren());
  Expression child = floatExpressionAtIndex(index);
  assert((child.type() == ExpressionNode::Type::Float &&
          sizeof(T) == sizeof(float)) ||
         (child.type() == ExpressionNode::Type::Double &&
          sizeof(T) == sizeof(double)));
  static_cast<Float<T> &>(child).setValue(value);
}

template <typename T>
void FloatList<T>::removeValueAtIndex(int index) {
  assert(index < numberOfChildren());
  List::removeChildAtIndexInPlace(index);
}

template <typename T>
T FloatList<T>::valueAtIndex(int index) const {
  if (index >= numberOfChildren()) {
    return NAN;
  }
  Expression child = floatExpressionAtIndex(index);
  assert((child.type() == ExpressionNode::Type::Float &&
          sizeof(T) == sizeof(float)) ||
         (child.type() == ExpressionNode::Type::Double &&
          sizeof(T) == sizeof(double)));
  return static_cast<Float<T> &>(child).value();
}

template <typename T>
Expression FloatList<T>::floatExpressionAtIndex(int index) const {
  assert(index >= 0 && index < numberOfChildren() && numberOfChildren() > 0);
  assert((childAtIndex(index).type() == ExpressionNode::Type::Float &&
          sizeof(T) == sizeof(float)) ||
         (childAtIndex(index).type() == ExpressionNode::Type::Double &&
          sizeof(T) == sizeof(double)));
  char *firstChild = reinterpret_cast<char *>(node()) +
                     Helpers::AlignedSize(sizeof(ListNode), ByteAlignment);
  return Expression(reinterpret_cast<ExpressionNode *>(
      firstChild + index * sizeof(FloatNode<T>)));
}

template class FloatList<float>;
template class FloatList<double>;

}  // namespace Poincare
