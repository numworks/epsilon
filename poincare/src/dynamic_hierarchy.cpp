#include <poincare/dynamic_hierarchy.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

DynamicHierarchy::DynamicHierarchy() :
  Hierarchy(),
  m_operands(nullptr),
  m_numberOfOperands(0)
{
}

DynamicHierarchy::DynamicHierarchy(const Expression * const * operands, int numberOfOperands, bool cloneOperands) :
  Hierarchy(),
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

void DynamicHierarchy::removeOperandAtIndex(int i, bool deleteAfterRemoval) {
  if (deleteAfterRemoval) {
    delete m_operands[i];
  } else {
    const_cast<Expression *>(m_operands[i])->setParent(nullptr);
  }
  for (int j=i; j<m_numberOfOperands; j++) {
    m_operands[j] = m_operands[j+1];
  }
  m_numberOfOperands--;
}

int DynamicHierarchy::compareToSameTypeExpression(const Expression * e) const {
  int m = this->numberOfOperands();
  int n = e->numberOfOperands();
  for (int i = 1; i <= m; i++) {
    // The NULL node is the least node type.
    if (n < i) {
      return 1;
    }
    if (this->operand(m-i)->compareTo(e->operand(n-i)) != 0) {
      return this->operand(m-i)->compareTo(e->operand(n-i));
    }
  }
  // The NULL node is the least node type.
  if (n > m) {
    return -1;
  }
  return 0;
}

int DynamicHierarchy::compareToGreaterTypeExpression(const Expression * e) const {
  int m = numberOfOperands();
  if (m == 0) {
    return -1;
  }
  /* Compare e to last term of hierarchy. */
  if (operand(m-1)->compareTo(e) != 0) {
    return operand(m-1)->compareTo(e);
  }
  if (m > 1) {
    return 1;
  }
  return 0;
}

void DynamicHierarchy::sortChildren() {
  for (int i = numberOfOperands()-1; i > 0; i--) {
    bool isSorted = true;
    for (int j = 0; j < numberOfOperands()-1; j++) {
      if (operand(j)->compareTo(operand(j+1)) > 0) {
        swapOperands(j, j+1);
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

}
