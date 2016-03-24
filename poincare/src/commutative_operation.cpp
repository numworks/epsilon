#include <poincare/commutative_operation.h>
extern "C" {
#include <stdlib.h>
}

CommutativeOperation::CommutativeOperation(Expression ** operands, int numberOfOperands, bool cloneOperands) {
  m_numberOfOperands = numberOfOperands;
  m_operands = (Expression **)malloc(numberOfOperands*sizeof(Expression *));
  for (int i=0; i<numberOfOperands; i++) {
    if (cloneOperands) {
      m_operands[i] = operands[i]; //FIXME: Do the cloning!
    } else {
      m_operands[i] = operands[i];
    }
  }
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
