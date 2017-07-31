#include <poincare/binary_operation.h>
#include <poincare/complex_matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}

namespace Poincare {

BinaryOperation::BinaryOperation()
{
  m_operands[0] = nullptr;
  m_operands[1] = nullptr;
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
  if (m_operands[1] != nullptr) {
    delete m_operands[1];
  }
  if (m_operands[0] != nullptr) {
    delete m_operands[0];
  }
}

bool BinaryOperation::hasValidNumberOfArguments() const {
  return m_operands[0]->hasValidNumberOfArguments() && m_operands[1]->hasValidNumberOfArguments();
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

Evaluation * BinaryOperation::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * leftOperandEvalutation = m_operands[0]->evaluate(context, angleUnit);
  Evaluation * rightOperandEvalutation = m_operands[1]->evaluate(context, angleUnit);
  Evaluation * result = nullptr;
  if (leftOperandEvalutation->numberOfRows() == 1 && leftOperandEvalutation->numberOfColumns() == 1 && rightOperandEvalutation->numberOfRows() == 1 && rightOperandEvalutation->numberOfColumns() == 1) {
    result = computeOnComplexes(leftOperandEvalutation->complexOperand(0), rightOperandEvalutation->complexOperand(0));
  } else if (leftOperandEvalutation->numberOfRows() == 1 && leftOperandEvalutation->numberOfColumns() == 1) {
    result = computeOnComplexAndComplexMatrix(leftOperandEvalutation->complexOperand(0), rightOperandEvalutation);
  } else if (rightOperandEvalutation->numberOfRows() == 1 && rightOperandEvalutation->numberOfColumns() == 1) {
    result = computeOnComplexMatrixAndComplex(leftOperandEvalutation, rightOperandEvalutation->complexOperand(0));
  } else {
    result = computeOnNumericalMatrices(leftOperandEvalutation, rightOperandEvalutation);
  }
  delete leftOperandEvalutation;
  delete rightOperandEvalutation;
  if (result == nullptr) {
    result = new Complex(Complex::Float(NAN));
  }
  return result;
}

Evaluation * BinaryOperation::computeOnComplexAndComplexMatrix(const Complex * c, Evaluation * n) const {
  return computeOnComplexMatrixAndComplex(n, c);
}

Evaluation * BinaryOperation::computeOnComplexMatrixAndComplex(Evaluation * m, const Complex * d) const {
  Complex * operands = new Complex[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = privateCompute(*(m->complexOperand(i)), *d);
  }
  Evaluation * result = new ComplexMatrix(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

Evaluation * BinaryOperation::computeOnNumericalMatrices(Evaluation * m, Evaluation * n) const {
  if (m->numberOfRows() != n->numberOfRows() && m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Complex * operands = new Complex[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = privateCompute(*(m->complexOperand(i)), *(n->complexOperand(i)));
  }
  Evaluation * result = new ComplexMatrix(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

}

