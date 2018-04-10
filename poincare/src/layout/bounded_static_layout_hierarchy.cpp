#include <poincare/bounded_static_layout_hierarchy.h>
#include <poincare/expression_layout_array.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
BoundedStaticLayoutHierarchy<T>::BoundedStaticLayoutHierarchy() :
  StaticLayoutHierarchy<T>(),
  m_numberOfChildren(0)
{
}

template<int T>
BoundedStaticLayoutHierarchy<T>::BoundedStaticLayoutHierarchy(const ExpressionLayout * const * operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfChildren(numberOfOperands)
{
  StaticLayoutHierarchy<T>::build(operands, numberOfOperands, cloneOperands);
}

template<>
BoundedStaticLayoutHierarchy<2>::BoundedStaticLayoutHierarchy(const ExpressionLayout * e, bool cloneOperands) :
  BoundedStaticLayoutHierarchy((ExpressionLayout **)&e, 1, cloneOperands)
{
}

template<>
BoundedStaticLayoutHierarchy<2>::BoundedStaticLayoutHierarchy(const ExpressionLayout * e1, const ExpressionLayout * e2, bool cloneOperands) :
  BoundedStaticLayoutHierarchy(ExpressionLayoutArray(e1, e2).array(), 2, cloneOperands)
{
}

template class Poincare::BoundedStaticLayoutHierarchy<2>;

}
