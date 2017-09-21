#include <poincare/bounded_static_hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy() :
  StaticHierarchy<T>()
{
}

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy(Expression * const * operands, int numberOfOperands, bool cloneOperands)
{
  StaticHierarchy<T>::m_numberOfOperands = numberOfOperands;
  StaticHierarchy<T>::build(operands, numberOfOperands, cloneOperands);
}

template<int T>
bool BoundedStaticHierarchy<T>::hasValidNumberOfArguments() const {
  if (StaticHierarchy<T>::m_numberOfOperands <= 0 || StaticHierarchy<T>::m_numberOfOperands > T) {
    return false;
  }
  return Hierarchy::hasValidNumberOfArguments();
}

template class Poincare::BoundedStaticHierarchy<2>;

}
