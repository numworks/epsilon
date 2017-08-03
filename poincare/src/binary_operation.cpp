#include <poincare/binary_operation.h>
#include <poincare/complex_matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
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

template<typename T> Evaluation<T> * BinaryOperation::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * leftOperandEvalutation = m_operands[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * rightOperandEvalutation = m_operands[1]->evaluate<T>(context, angleUnit);
  Evaluation<T> * result = nullptr;
  if (leftOperandEvalutation->numberOfRows() == 1 && leftOperandEvalutation->numberOfColumns() == 1 && rightOperandEvalutation->numberOfRows() == 1 && rightOperandEvalutation->numberOfColumns() == 1) {
    result = new Complex<T>(privateCompute(*(leftOperandEvalutation->complexOperand(0)), *(rightOperandEvalutation->complexOperand(0))));
  } else if (leftOperandEvalutation->numberOfRows() == 1 && leftOperandEvalutation->numberOfColumns() == 1) {
    result = computeOnComplexAndComplexMatrix(leftOperandEvalutation->complexOperand(0), rightOperandEvalutation);
  } else if (rightOperandEvalutation->numberOfRows() == 1 && rightOperandEvalutation->numberOfColumns() == 1) {
    result = computeOnComplexMatrixAndComplex(leftOperandEvalutation, rightOperandEvalutation->complexOperand(0));
  } else {
    result = computeOnComplexMatrices(leftOperandEvalutation, rightOperandEvalutation);
  }
  delete leftOperandEvalutation;
  delete rightOperandEvalutation;
  if (result == nullptr) {
    result = new Complex<T>(Complex<T>::Float(NAN));
  }
  return result;
}

template<typename T> Evaluation<T> * BinaryOperation::templatedComputeOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n) const {
  return computeOnComplexMatrixAndComplex(n, c);
}

template<typename T> Evaluation<T> * BinaryOperation::templatedComputeOnComplexMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d) const {
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = privateCompute(*(m->complexOperand(i)), *d);
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

template<typename T> Evaluation<T> * BinaryOperation::templatedComputeOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n) const {
  if (m->numberOfRows() != n->numberOfRows() && m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = privateCompute(*(m->complexOperand(i)), *(n->complexOperand(i)));
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

}

template Poincare::Evaluation<float>* Poincare::BinaryOperation::templatedComputeOnComplexAndComplexMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*) const;
template Poincare::Evaluation<double>* Poincare::BinaryOperation::templatedComputeOnComplexAndComplexMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*) const;
