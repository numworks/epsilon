#include <poincare/static_hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
StaticHierarchy<T>::StaticHierarchy() :
  m_operands{}
{
}

template<int T>
StaticHierarchy<T>::StaticHierarchy(Expression * const * operands, bool cloneOperands)
{
  build(operands, T, cloneOperands);
}

template<int T>
StaticHierarchy<T>::~StaticHierarchy() {
  for (int i = 0; i < T; i++) {
    if (m_operands[i] != nullptr) {
      delete m_operands[i];
    }
  }
}

template<int T>
void StaticHierarchy<T>::setArgument(ListData * listData, int numberOfOperands, bool clone) {
  build(listData->operands(), listData->numberOfOperands(), clone);
}

template<int T>
bool StaticHierarchy<T>::hasValidNumberOfOperands(int numberOfOperands) const {
  return numberOfOperands == T;
}

template<int T>
void StaticHierarchy<T>::build(Expression * const * operands, int numberOfOperands, bool cloneOperands) {
  assert(operands != nullptr);
  assert(numberOfOperands <= T);
  for (int i=0; i < numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
  }
}

template class Poincare::StaticHierarchy<0>;
template class Poincare::StaticHierarchy<1>;
template class Poincare::StaticHierarchy<2>;
template class Poincare::StaticHierarchy<3>;

}
