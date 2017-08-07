extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/expression_matrix.h>
#include <poincare/complex_matrix.h>
#include <poincare/complex.h>
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

ExpressionMatrix::ExpressionMatrix(MatrixData * matrixData) :
  m_matrixData(matrixData)
{
}

ExpressionMatrix::ExpressionMatrix(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands)
{
  m_matrixData = new MatrixData(newOperands, numberOfOperands, numberOfColumns, numberOfRows, cloneOperands);
}

ExpressionMatrix::~ExpressionMatrix() {
  delete m_matrixData;
}

bool ExpressionMatrix::hasValidNumberOfArguments() const {
  for (int i = 0; i < numberOfOperands(); i++) {
    if (!operand(i)->hasValidNumberOfArguments()) {
      return false;
    }
  }
  return true;
}


int ExpressionMatrix::numberOfRows() const {
  return m_matrixData->numberOfRows();
}

int ExpressionMatrix::numberOfColumns() const {
  return m_matrixData->numberOfColumns();
}

const Expression * ExpressionMatrix::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  return m_matrixData->operands()[i];
}

Expression * ExpressionMatrix::clone() const {
  return this->cloneWithDifferentOperands(m_matrixData->operands(), numberOfOperands(), true);
}

Expression::Type ExpressionMatrix::type() const {
  return Type::ExpressionMatrix;
}

Expression * ExpressionMatrix::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new ExpressionMatrix(newOperands, numberOfOperands, numberOfColumns(), numberOfRows(), cloneOperands);
}

Evaluation * ExpressionMatrix::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Complex * operands = new Complex[numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    Evaluation * operandEvaluation = operand(i)->evaluate(context, angleUnit);
    if (operandEvaluation->numberOfOperands() != 1) {
      operands[i] = Complex::Float(NAN);
    } else {
      operands[i] = *(operandEvaluation->complexOperand(0));
    }
    delete operandEvaluation;
  }
  Evaluation * matrix = new ComplexMatrix(operands, numberOfRows(), numberOfColumns());
  delete[] operands;
  return matrix;
}

Complex * ExpressionMatrix::defaultExpression() {
  static Complex * defaultExpression = new Complex(Complex::Float(0.0f));
  return defaultExpression;
}

}
