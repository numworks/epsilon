#include <poincare/dynamic_hierarchy.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

DynamicHierarchy::DynamicHierarchy() :
  m_operands(nullptr),
  m_numberOfOperands(0)
{
}

DynamicHierarchy::DynamicHierarchy(Expression * const * operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfOperands(numberOfOperands)
{
  assert(operands != nullptr);
  assert(numberOfOperands >= 2);
  m_operands = new Expression * [numberOfOperands];
  for (int i=0; i<numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
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

void DynamicHierarchy::addOperands(Expression * const * operands, int numberOfOperands) {
  assert(numberOfOperands > 0);
  Expression ** newOperands = new Expression * [m_numberOfOperands+numberOfOperands];
  for (int i=0; i<m_numberOfOperands; i++) {
    newOperands[i] = m_operands[i];
  }
  for (int i=0; i<numberOfOperands; i++) {
    newOperands[i+m_numberOfOperands] = operands[i];
  }
  delete[] m_operands;
  m_operands = newOperands;
  m_numberOfOperands += numberOfOperands;
}

void DynamicHierarchy::removeOperand(const Expression * e, bool deleteAfterRemoval) {
  for (int i=0; i<numberOfOperands(); i++) {
    if (operand(i) == e) {
      if (deleteAfterRemoval) {
        delete m_operands[i];
      }
      for (int j=i; j<m_numberOfOperands; j++) {
        m_operands[j] = m_operands[j+1];
      }
      m_numberOfOperands--;
      break;
    }
  }
}

}
