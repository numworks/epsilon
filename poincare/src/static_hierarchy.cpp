#include <poincare/static_hierarchy.h>
#include <poincare/expression_array.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
StaticHierarchy<T>::StaticHierarchy() :
  Expression(),
  m_operands{}
{
}

template<int T>
StaticHierarchy<T>::StaticHierarchy(const Expression * const * operands, bool cloneOperands) :
  Expression()
{
  build(operands, T, cloneOperands);
}

template<>
StaticHierarchy<1>::StaticHierarchy(const Expression * e, bool cloneOperands) :
  StaticHierarchy((Expression **)&e, cloneOperands)
{
}

template<>
StaticHierarchy<2>::StaticHierarchy(const Expression * e1, const Expression * e2, bool cloneOperands) :
  StaticHierarchy(ExpressionArray(e1, e2).array(), cloneOperands)
{
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
void StaticHierarchy<T>::setArgument(ListData * listData, int numberOfChildren, bool clone) {
  build(listData->operands(), listData->numberOfChildren(), clone);
}

template<int T>
bool StaticHierarchy<T>::hasValidNumberOfOperands(int numberOfChildren) const {
  return numberOfChildren == T;
}

template<int T>
void StaticHierarchy<T>::build(const Expression * const * operands, int numberOfChildren, bool cloneOperands) {
  assert(operands != nullptr);
  assert(numberOfChildren <= T);
  for (int i=0; i < numberOfChildren; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
    const_cast<Expression *>(m_operands[i])->setParent(this);
  }
}

template<int T>
int StaticHierarchy<T>::simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const {
  for (int i = 0; i < this->numberOfChildren(); i++) {
    // The NULL node is the least node type.
    if (e->numberOfChildren() <= i) {
      return 1;
    }
    if (SimplificationOrder(this->operand(i), e->operand(i), canBeInterrupted) != 0) {
      return SimplificationOrder(this->operand(i), e->operand(i), canBeInterrupted);
    }
  }
  // The NULL node is the least node type.
  if (e->numberOfChildren() > numberOfChildren()) {
    return -1;
  }
  return 0;
}

template class Poincare::StaticHierarchy<0>;
template class Poincare::StaticHierarchy<1>;
template class Poincare::StaticHierarchy<2>;
template class Poincare::StaticHierarchy<3>;

}
