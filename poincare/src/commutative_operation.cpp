#include <poincare/commutative_operation.h>
extern "C" {
#include <stdlib.h>
}

CommutativeOperation::CommutativeOperation(Expression * first_operand, Expression * second_operand) {
  m_numberOfOperands = 2;
  m_operands = (Expression **)malloc(2*sizeof(Expression *));
  m_operands[0] = first_operand;
  m_operands[1] = second_operand;
}

CommutativeOperation::~CommutativeOperation() {
  for (int i=0; i<m_numberOfOperands; i++) {
    delete m_operands[i];
  }
  free(m_operands);
}

int CommutativeOperation::numberOfOperands() {
  return m_numberOfOperands;
}

Expression * CommutativeOperation::operand(int i) {
  return m_operands[i];
}

float CommutativeOperation::approximate(Context& context) {
  float result = m_operands[0]->approximate(context);
  for (size_t i=1; i<m_numberOfOperands; i++) {
    float next = m_operands[i]->approximate(context);
    result = this->operateApproximatevelyOn(result, next);
  }
  return result;
}
