#include <poincare/bounded_static_hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy() :
  StaticHierarchy<T>(),
  m_numberOfOperands(0)
{
}

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy(const Expression * const * operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfOperands(numberOfOperands)
{
  StaticHierarchy<T>::build(operands, numberOfOperands, cloneOperands);
}

template<int T>
void BoundedStaticHierarchy<T>::setArgument(ListData * listData, int numberOfOperands, bool clone) {
  StaticHierarchy<T>::setArgument(listData, numberOfOperands, clone);
  m_numberOfOperands = listData->numberOfOperands();
}

template<int T>
bool BoundedStaticHierarchy<T>::hasValidNumberOfOperands(int numberOfOperands) const {
  return numberOfOperands >= 1 && numberOfOperands <= T;
}

template class Poincare::BoundedStaticHierarchy<2>;

}
