#include <poincare/binary_operation.h>
extern "C" {
#include <assert.h>
}

BinaryOperation::BinaryOperation(Expression ** operands, bool cloneOperands) {
  assert(operands != nullptr);
  assert(operands[0] != nullptr);
  assert(operands[1] != nullptr);
  if (cloneOperands) {
    m_operands[0] = operands[0]->clone();
    m_operands[1] = operands[1]->clone();
  } else {
    m_operands[0] = operands[0];
    m_operands[1] = operands[1];
  }
}

BinaryOperation::~BinaryOperation() {
  delete m_operands[1];
  delete m_operands[0];
}

int BinaryOperation::numberOfOperands() {
  return 2;
}

Expression * BinaryOperation::operand(int i) {
  assert(i>0 && i<=2);
  return m_operands[i];
}

Expression * BinaryOperation::clone() {
  return this->cloneWithDifferentOperands(m_operands, 2, true);
}
