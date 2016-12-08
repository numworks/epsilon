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

int BinaryOperation::numberOfOperands() const {
  return 2;
}

const Expression * BinaryOperation::operand(int i) const {
  assert(i >= 0);
  assert(i < 2);
  return m_operands[i];
}

Expression * BinaryOperation::clone() const {
  return this->cloneWithDifferentOperands((Expression**) m_operands, 2, true);
}

Expression * BinaryOperation::evaluate(Context& context) const {
  Expression * leftOperandEvalutation = m_operands[0]->evaluate(context);
  Expression * rightOperandEvalutation = m_operands[1]->evaluate(context);
  if (leftOperandEvalutation == nullptr || rightOperandEvalutation == nullptr) {
    return nullptr;
  }
  Expression * result = nullptr;
  if (leftOperandEvalutation->type() == Type::Float && rightOperandEvalutation->type() == Type::Float) {
    result = new Float(this->approximate(context));
  }
  if (leftOperandEvalutation->type() == Type::Matrix && rightOperandEvalutation->type() == Type::Float) {
    result = evaluateOnMatrixAndFloat((Matrix *)leftOperandEvalutation, (Float *)rightOperandEvalutation, context);
  }
  if (leftOperandEvalutation->type() == Type::Float && rightOperandEvalutation->type() == Type::Matrix) {
    result =  evaluateOnFloatAndMatrix((Float *)leftOperandEvalutation, (Matrix *)rightOperandEvalutation, context);
  }
  if (leftOperandEvalutation->type() == Type::Matrix && rightOperandEvalutation->type() == Type::Matrix) {
    result = evaluateOnMatrices((Matrix *)leftOperandEvalutation, (Matrix *)rightOperandEvalutation, context);
  }
  delete leftOperandEvalutation;
  delete rightOperandEvalutation;
  return result;
}

Expression * BinaryOperation::evaluateOnMatrixAndFloat(Matrix * m, Float * a, Context& context) const {
  return nullptr;
}

Expression * BinaryOperation::evaluateOnFloatAndMatrix(Float * a, Matrix * m, Context& context) const {
  return nullptr;
}

Expression * BinaryOperation::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context) const {
  return nullptr;
}
