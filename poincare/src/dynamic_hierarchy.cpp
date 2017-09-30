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
  assert(numberOfOperands >= 2);
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

void DynamicHierarchy::removeOperand(const Expression * e, bool deleteAfterRemoval) {
  for (int i=0; i<numberOfOperands(); i++) {
    if (operand(i) == e) {
      removeOperandAtIndex(i, deleteAfterRemoval);
      break;
    }
  }
}

void DynamicHierarchy::removeOperandAtIndex(int i, bool deleteAfterRemoval) {
  //FIXME: Do something here if we don't want to have a single child
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

}
