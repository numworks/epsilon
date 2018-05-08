#include <poincare/dynamic_hierarchy.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

DynamicHierarchy::DynamicHierarchy() :
  Expression(),
  m_operands(nullptr),
  m_numberOfOperands(0)
{
}

DynamicHierarchy::DynamicHierarchy(const Expression * const * operands, int numberOfOperands, bool cloneOperands) :
  Expression(),
  m_numberOfOperands(numberOfOperands)
{
  assert(operands != nullptr);
  m_operands = new const Expression * [numberOfOperands];
  for (int i=0; i<numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
    const_cast<Expression *>(m_operands[i])->setParent(this);
  }
}

DynamicHierarchy::~DynamicHierarchy() {
  if (m_operands != nullptr) {
    for (int i = 0; i < m_numberOfOperands; i++) {
      if (m_operands[i] != nullptr) {
        delete m_operands[i];
      }
    }
  }
  delete[] m_operands;
}

void DynamicHierarchy::addOperands(const Expression * const * operands, int numberOfOperands) {
  assert(numberOfOperands > 0);
  const Expression ** newOperands = new const Expression * [m_numberOfOperands+numberOfOperands];
  for (int i=0; i<m_numberOfOperands; i++) {
    newOperands[i] = m_operands[i];
  }
  for (int i=0; i<numberOfOperands; i++) {
    const_cast<Expression *>(operands[i])->setParent(this);
    newOperands[i+m_numberOfOperands] = operands[i];
  }
  delete[] m_operands;
  m_operands = newOperands;
  m_numberOfOperands += numberOfOperands;
}

void DynamicHierarchy::mergeOperands(DynamicHierarchy * d) {
  removeOperand(d, false);
  addOperands(d->operands(), d->numberOfOperands());
  d->detachOperands();
  delete d;
}

void DynamicHierarchy::addOperand(Expression * operand) {
  addOperandAtIndex(operand, m_numberOfOperands);
}

void DynamicHierarchy::addOperandAtIndex(Expression * operand, int index) {
  assert(index >= 0 && index <= m_numberOfOperands);
  const Expression ** newOperands = new const Expression * [m_numberOfOperands+1];
  int j = 0;
  for (int i=0; i<=m_numberOfOperands; i++) {
    if (i == index) {
      operand->setParent(this);
      newOperands[i] = operand;
    } else {
      newOperands[i] = m_operands[j++];
    }
  }
  delete[] m_operands;
  m_operands = newOperands;
  m_numberOfOperands += 1;
}

void DynamicHierarchy::removeOperand(const Expression * e, bool deleteAfterRemoval) {
  for (int i=0; i<numberOfOperands(); i++) {
    if (operand(i) == e) {
      removeOperandAtIndex(i, deleteAfterRemoval);
      break;
    }
  }
}




void DynamicHierarchy::sortOperands(ExpressionOrder order, bool canBeInterrupted) {
  for (int i = numberOfOperands()-1; i > 0; i--) {
    bool isSorted = true;
    for (int j = 0; j < numberOfOperands()-1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
#if MATRIX_EXACT_REDUCING
      if (order(operand(j), operand(j+1), canBeInterrupted) > 0 && (!operand(j)->recursivelyMatches(Expression::IsMatrix) || !operand(j+1)->recursivelyMatches(Expression::IsMatrix))) {
#else
      if (order(operand(j), operand(j+1), canBeInterrupted) > 0) {
#endif
        swapOperands(j, j+1);
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

Expression * DynamicHierarchy::squashUnaryHierarchy() {
  if (numberOfOperands() == 1) {
    assert(parent() != nullptr);
    Expression * o = editableOperand(0);
    replaceWith(o, true);
    return o;
  }
  return this;
}

// Private

void DynamicHierarchy::removeOperandAtIndex(int i, bool deleteAfterRemoval) {
  if (deleteAfterRemoval) {
    delete m_operands[i];
  } else {
    const_cast<Expression *>(m_operands[i])->setParent(nullptr);
  }
  m_numberOfOperands--;
  for (int j=i; j<m_numberOfOperands; j++) {
    m_operands[j] = m_operands[j+1];
  }
}

int DynamicHierarchy::simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const {
  int m = this->numberOfOperands();
  int n = e->numberOfOperands();
  for (int i = 1; i <= m; i++) {
    // The NULL node is the least node type.
    if (n < i) {
      return 1;
    }
    int order = SimplificationOrder(this->operand(m-i), e->operand(n-i), canBeInterrupted);
    if (order != 0) {
      return order;
    }
  }
  // The NULL node is the least node type.
  if (n > m) {
    return -1;
  }
  return 0;
}

int DynamicHierarchy::simplificationOrderGreaterType(const Expression * e, bool canBeInterrupted) const {
  int m = numberOfOperands();
  if (m == 0) {
    return -1;
  }
  /* Compare e to last term of hierarchy. */
  int order = SimplificationOrder(operand(m-1), e, canBeInterrupted);
  if (order != 0) {
    return order;
  }
  if (m > 1) {
    return 1;
  }
  return 0;
}

}
