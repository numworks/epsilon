#include <poincare/hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

Hierarchy::Hierarchy(int numberOfOperands) :
  m_numberOfOperands(numberOfOperands)
{
}

void Hierarchy::swapOperands(int i, int j) {
  assert(i < numberOfOperands());
  assert(j < numberOfOperands());
  Expression ** op = operands();
  Expression * temp = op[i];
  op[i] = op[j];
  op[j] = temp;
}

void Hierarchy::replaceOperand(Expression * oldChild, Expression * newChild) {
  Expression ** op = operands();
  for (int i=0; i<m_numberOfOperands; i++) {
    if (op[i] == oldChild) {
      delete oldChild;
      op[i] = newChild;
      break;
    }
  }
}

void Hierarchy::removeOperand(Expression * oldChild) {
  Expression ** op = operands();
  for (int i=0; i<m_numberOfOperands; i++) {
    if (op[i] == oldChild) {
      delete oldChild;
      m_numberOfOperands--;
      for (int j=i; j<m_numberOfOperands; j++) {
        op[j] = op[j+1];
      }
      break;
    }
  }
}

void Hierarchy::sort() {
  // First, sort every child
  Expression::sort();
  // Second, sort all children together if the expression is commutative
  if (!isCommutative()) {
    return;
  }
  // TODO: use a heap sort instead of a buble sort
  for (int i = numberOfOperands()-1; i > 0; i--) {
    bool isSorted = true;
    for (int j = 0; j < numberOfOperands()-1; j++) {
      if (operand(j)->comparesTo(operand(j+1)) > 0) {
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
