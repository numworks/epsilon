#include <poincare/old/float.h>
#include <poincare/old/float_list.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

using namespace Internal;

template <typename T>
void FloatList<T>::addValueAtIndex(T value, int index) {
  assert(index <= numberOfChildren());
  List::addChildAtIndexInPlace(JuniorExpression::Builder<T>(value), index,
                               numberOfChildren());
}

template <typename T>
void FloatList<T>::replaceValueAtIndex(T value, int index) {
  assert(index < tree()->numberOfChildren());
  Tree *child = floatNodeAtIndex(index);
  child->nodeValueBlock(0)->set<T>(value);
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
  const Tree *child = floatNodeAtIndex(index);
  return child->nodeValueBlock(0)->get<T>();
}

template <typename T>
Tree *FloatList<T>::floatNodeAtIndex(int index) const {
  constexpr Type type =
      sizeof(T) == sizeof(float) ? Type::SingleFloat : Type::DoubleFloat;
  constexpr int k_nodeSize = TypeBlock(type).nodeSize();
  assert(index >= 0 && index < tree()->numberOfChildren());
  assert(tree()->child(index)->type() == type);
  const Tree *firstChild = tree()->nextNode();
  return const_cast<Tree *>(firstChild + index * k_nodeSize);
}

template class FloatList<float>;
template class FloatList<double>;

}  // namespace Poincare
