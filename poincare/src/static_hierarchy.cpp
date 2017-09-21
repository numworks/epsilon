#include <poincare/static_hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
StaticHierarchy<T>::StaticHierarchy() :
  Hierarchy(0),
  m_operands{}
{
}

template<int T>
StaticHierarchy<T>::StaticHierarchy(Expression * const * operands, bool cloneOperands) :
  Hierarchy(T)
{
  build(operands, T, cloneOperands);
}

template<int T>
StaticHierarchy<T>::~StaticHierarchy() {
  for (int i = 0; i < numberOfOperands(); i++) {
    delete m_operands[i];
  }
}

template<int T>
void StaticHierarchy<T>::setArgument(ListData * listData, int numberOfOperands, bool clone) {
  build(listData->operands(), listData->numberOfOperands(), clone);
  m_numberOfOperands = numberOfOperands;
}

template<int T>
int StaticHierarchy<T>::numberOfOperands() const {
  return m_numberOfOperands > T ? T : m_numberOfOperands;
}

template<int T>
const Expression * StaticHierarchy<T>::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  return m_operands[i];
}

template<int T>
bool StaticHierarchy<T>::hasValidNumberOfArguments() const {
  if (T != m_numberOfOperands) {
    return false;
  }
  return Hierarchy::hasValidNumberOfArguments();
}

template<int T>
void StaticHierarchy<T>::build(Expression * const * operands, int numberOfOperands, bool cloneOperands) {
  assert(operands != nullptr);
  int clippedNumberOfOperands = numberOfOperands > T ? T : numberOfOperands;
  for (int i=0; i<clippedNumberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
  }
}

template<int T>
Expression ** StaticHierarchy<T>::operands() {
  return m_operands;
}

template class Poincare::StaticHierarchy<0>;
template class Poincare::StaticHierarchy<1>;
template class Poincare::StaticHierarchy<2>;
template class Poincare::StaticHierarchy<3>;

}
