#include <poincare/bounded_static_hierarchy.h>
#include <poincare/expression_array.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
BoundedStaticHierarchy<T>::BoundedStaticHierarchy(const Expression * const * operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfOperands(numberOfOperands)
{
  StaticHierarchy<T>::build(operands, numberOfOperands, cloneOperands);
}

template<>
BoundedStaticHierarchy<2>::BoundedStaticHierarchy(const Expression * e1, const Expression * e2, bool cloneOperands) :
  BoundedStaticHierarchy(ExpressionArray(e1, e2).array(), 2, cloneOperands)
{
}

template<int T>
void BoundedStaticHierarchy<T>::setArgument(ListData * listData, int numberOfOperands, bool clone) {
  StaticHierarchy<T>::setArgument(listData, numberOfOperands, clone);
  m_numberOfOperands = listData->numberOfOperands();
}

template class Poincare::BoundedStaticHierarchy<2>;

}
