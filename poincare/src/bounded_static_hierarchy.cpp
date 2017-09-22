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
BoundedStaticHierarchy<T>::BoundedStaticHierarchy(Expression * const * operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfOperands(numberOfOperands)
{
  StaticHierarchy<T>::build(operands, numberOfOperands, cloneOperands);
}

template<int T>
bool BoundedStaticHierarchy<T>::hasValidNumberOfArguments() const {
  return true;
}

template class Poincare::BoundedStaticHierarchy<2>;

}
