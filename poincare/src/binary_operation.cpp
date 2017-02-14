#include <poincare/binary_operation.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

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

Expression * BinaryOperation::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * leftOperandEvalutation = m_operands[0]->evaluate(context, angleUnit);
  Expression * rightOperandEvalutation = m_operands[1]->evaluate(context, angleUnit);
  if (leftOperandEvalutation == nullptr || rightOperandEvalutation == nullptr) {
    return nullptr;
  }
  Expression * result = nullptr;
  if (leftOperandEvalutation->type() == Type::Complex && rightOperandEvalutation->type() == Type::Complex) {
    result = evaluateOnComplex((Complex *)leftOperandEvalutation, (Complex *)rightOperandEvalutation, context, angleUnit);
  }
  if (leftOperandEvalutation->type() == Type::Complex && rightOperandEvalutation->type() == Type::Matrix) {
    result =  evaluateOnComplexAndMatrix((Complex *)leftOperandEvalutation, (Matrix *)rightOperandEvalutation, context, angleUnit);
  }
    if (leftOperandEvalutation->type() == Type::Matrix && rightOperandEvalutation->type() == Type::Complex) {
    result = evaluateOnMatrixAndComplex((Matrix *)leftOperandEvalutation, (Complex *)rightOperandEvalutation, context, angleUnit);
  }
  if (leftOperandEvalutation->type() == Type::Matrix && rightOperandEvalutation->type() == Type::Matrix) {
    result = evaluateOnMatrices((Matrix *)leftOperandEvalutation, (Matrix *)rightOperandEvalutation, context, angleUnit);
  }
  delete leftOperandEvalutation;
  delete rightOperandEvalutation;
  return result;
}

Expression * BinaryOperation::evaluateOnMatrixAndComplex(Matrix * m, Complex * c, Context& context, AngleUnit angleUnit) const {
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    Expression * evaluation = m->operand(i)->evaluate(context, angleUnit);
    assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
    if (evaluation->type() == Type::Matrix) {
      operands[i] = new Complex(NAN);
      delete evaluation;
      continue;
    }
    operands[i] = evaluateOnComplex((Complex *)evaluation, c, context, angleUnit);
    delete evaluation;
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}

Expression * BinaryOperation::evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const {
  return evaluateOnMatrixAndComplex(m, c, context, angleUnit);
}

Expression * BinaryOperation::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  if (m->numberOfColumns() != n->numberOfColumns() || m->numberOfRows() != n->numberOfRows()) {
    return nullptr;
  }
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    Expression * mEvaluation = m->operand(i)->evaluate(context, angleUnit);
    Expression * nEvaluation = n->operand(i)->evaluate(context, angleUnit);
    assert(mEvaluation->type() == Type::Matrix || mEvaluation->type() == Type::Complex);
    assert(nEvaluation->type() == Type::Matrix || nEvaluation->type() == Type::Complex);
    if (mEvaluation->type() == Type::Matrix ||nEvaluation->type() == Type::Matrix) {
      operands[i] = new Complex(NAN);
      delete mEvaluation;
      delete nEvaluation;
      continue;
    }
    operands[i] = evaluateOnComplex((Complex *)mEvaluation, (Complex *)nEvaluation, context, angleUnit);
    delete mEvaluation;
    delete nEvaluation;
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}

}

