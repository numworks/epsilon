#include <poincare/commutative_operation.h>

namespace Poincare {

void CommutativeOperation::sort() {
  // First, sort every child
  for (int i = 0; i < m_numberOfOperands; i++) {
    m_operands[i]->sort();
  }
  // Second, sort all children together
  // TODO: use a heap sort instead of a buble sort
  for (int i = m_numberOfOperands-1; i > 0; i--) {
    bool isSorted = true;
    for (int j = 0; j < m_numberOfOperands-1; j++) {
      if (m_operands[j]->isGreaterThan(m_operands[j+1])) {
        Expression * temp =  m_operands[j];
        m_operands[j] = m_operands[j+1];
        m_operands[j+1] = temp;
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

}
