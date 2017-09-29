#include <poincare/hierarchy.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

const Expression * Hierarchy::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  assert(operands()[i]->parent() == nullptr || operands()[i]->parent() == this);
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

void Hierarchy::detachOperand(const Expression * e) {
  for (int i=0; i<numberOfOperands(); i++) {
    if (operand(i) == e) {
      detachOperandAtIndex(i);
    }
  }
}

void Hierarchy::detachOperands() {
  for (int i=0; i<numberOfOperands(); i++) {
    detachOperandAtIndex(i);
  }
}

void Hierarchy::replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand) {
  Expression ** op = const_cast<Expression **>(operands());
  for (int i=0; i<numberOfOperands(); i++) {
    if (op[i] == oldOperand) {
      if (oldOperand != nullptr) {
        const_cast<Expression *>(oldOperand)->setParent(nullptr);
      }
      if (deleteOldOperand) {
        delete oldOperand;
      }
      if (newOperand != nullptr) {
        const_cast<Expression *>(newOperand)->setParent(this);
      }
      op[i] = newOperand;
      break;
    }
  }
}

void Hierarchy::detachOperandAtIndex(int i) {
  Expression ** op = const_cast<Expression **>(operands());
  // When detachOperands is called, it's very likely that said operands have been stolen
  if (op[i]->parent() == this) {
    const_cast<Expression *>(op[i])->setParent(nullptr);
  }
  op[i] = nullptr;
}

}
