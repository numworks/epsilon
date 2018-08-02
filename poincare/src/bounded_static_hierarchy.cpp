#include <poincare/bounded_static_hierarchy.h>
#include <poincare/expression_array.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy() :
  StaticHierarchy<T>(),
  m_numberOfChildren(0)
{
}

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy(const Expression * const * operands, int numberOfChildren, bool cloneOperands) :
  m_numberOfChildren(numberOfChildren)
{
  StaticHierarchy<T>::build(operands, numberOfChildren, cloneOperands);
}

template<>
BoundedStaticHierarchy<2>::BoundedStaticHierarchy(const Expression * e1, const Expression * e2, bool cloneOperands) :
  BoundedStaticHierarchy(ExpressionArray(e1, e2).array(), 2, cloneOperands)
{
}

template<>
BoundedStaticHierarchy<2>::BoundedStaticHierarchy(const Expression * e, bool cloneOperands) :
  BoundedStaticHierarchy((Expression **)&e, 1, cloneOperands)
{
}

template<int T>
void BoundedStaticHierarchy<T>::setArgument(ListData * listData, int numberOfChildren, bool clone) {
  StaticHierarchy<T>::setArgument(listData, numberOfChildren, clone);
  m_numberOfChildren = listData->numberOfChildren();
}

template<int T>
bool BoundedStaticHierarchy<T>::hasValidNumberOfOperands(int numberOfChildren) const {
  return numberOfChildren >= 1 && numberOfChildren <= T;
}

template class Poincare::BoundedStaticHierarchy<2>;

}
