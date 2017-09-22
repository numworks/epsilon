#include <poincare/hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

const Expression * Hierarchy::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  return operands()[i];
}

void Hierarchy::swapOperands(int i, int j) {
  assert(i >= 0 && i < numberOfOperands());
  assert(j >= 0 && j < numberOfOperands());
  Expression ** op = const_cast<Expression **>(operands());
  Expression * temp = op[i];
  op[i] = op[j];
  op[j] = temp;
}

void Hierarchy::detachOperands() {
  Expression ** op = const_cast<Expression **>(operands());
  for (int i=0; i<numberOfOperands(); i++) {
    op[i] = nullptr;
  }
}

void Hierarchy::replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand) {
  Expression ** op = const_cast<Expression **>(operands());
  for (int i=0; i<numberOfOperands(); i++) {
    if (op[i] == oldOperand) {
      if (deleteOldOperand) {
        delete oldOperand;
      }
      op[i] = newOperand;
      break;
    }
  }
}

}
