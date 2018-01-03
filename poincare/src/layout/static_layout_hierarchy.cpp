#include <poincare/static_layout_hierarchy.h>
#include <poincare/expression_layout_array.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
StaticLayoutHierarchy<T>::StaticLayoutHierarchy() :
  ExpressionLayout(),
  m_children{}
{
}

template<int T>
StaticLayoutHierarchy<T>::StaticLayoutHierarchy(const ExpressionLayout * const * children, bool cloneChildren) :
  ExpressionLayout()
{
  build(children, T, cloneChildren);
}

template<>
StaticLayoutHierarchy<1>::StaticLayoutHierarchy(const ExpressionLayout * e, bool cloneChildren) :
  StaticLayoutHierarchy((ExpressionLayout **)&e, cloneChildren)
{
}

template<>
StaticLayoutHierarchy<2>::StaticLayoutHierarchy(const ExpressionLayout * e1, const ExpressionLayout * e2, bool cloneChildren) :
  StaticLayoutHierarchy(ExpressionLayoutArray(e1, e2).array(), cloneChildren)
{
}

template<>
StaticLayoutHierarchy<3>::StaticLayoutHierarchy(const ExpressionLayout * e1, const ExpressionLayout * e2, const ExpressionLayout * e3, bool cloneChildren) :
  StaticLayoutHierarchy(ExpressionLayoutArray(e1, e2, e3).array(), cloneChildren)
{
}

template<int T>
StaticLayoutHierarchy<T>::~StaticLayoutHierarchy() {
  for (int i = 0; i < T; i++) {
    if (m_children[i] != nullptr) {
      delete m_children[i];
    }
  }
}

template<int T>
void StaticLayoutHierarchy<T>::build(const ExpressionLayout * const * operands, int numberOfOperands, bool cloneOperands) {
  assert(operands != nullptr);
  assert(numberOfOperands <= T);
  for (int i=0; i < numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_children[i] = operands[i]->clone();
    } else {
      m_children[i] = operands[i];
    }
    const_cast<ExpressionLayout *>(m_children[i])->setParent(this);
  }
}

template class Poincare::StaticLayoutHierarchy<0>;
template class Poincare::StaticLayoutHierarchy<1>;
template class Poincare::StaticLayoutHierarchy<2>;
template class Poincare::StaticLayoutHierarchy<3>;

}

