#include <poincare/static_hierarchy.h>
#include <poincare/expression_array.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<int T>
StaticHierarchy<T>::StaticHierarchy(const Expression * const * operands, bool cloneOperands) :
  Expression()
{
  build(operands, T, cloneOperands);
}

template<int T>
void StaticHierarchy<T>::setArgument(ListData * listData, int numberOfOperands, bool clone) {
  build(listData->operands(), listData->numberOfOperands(), clone);
}

template<int T>
void StaticHierarchy<T>::build(const Expression * const * operands, int numberOfOperands, bool cloneOperands) {
  assert(operands != nullptr);
  assert(numberOfOperands <= T);
  for (int i=0; i < numberOfOperands; i++) {
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
  for (int i = 0; i < this->numberOfOperands(); i++) {
    // The NULL node is the least node type.
    if (e->numberOfOperands() <= i) {
      return 1;
    }
    if (SimplificationOrder(this->operand(i), e->operand(i), canBeInterrupted) != 0) {
      return SimplificationOrder(this->operand(i), e->operand(i), canBeInterrupted);
    }
  }
  // The NULL node is the least node type.
  if (e->numberOfOperands() > numberOfOperands()) {
    return -1;
  }
  return 0;
}

template class Poincare::StaticHierarchy<0>;
template class Poincare::StaticHierarchy<1>;
template class Poincare::StaticHierarchy<2>;
template class Poincare::StaticHierarchy<3>;

}
