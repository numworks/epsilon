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

ExpressionMatrix::ExpressionMatrix(Expression ** newOperands, int numberOfOperands, int numberOfRows, int numberOfColumns, bool cloneOperands)
{
  m_matrixData = new MatrixData(newOperands, numberOfOperands, numberOfRows, numberOfColumns, cloneOperands);
}

ExpressionMatrix::~ExpressionMatrix() {
  delete m_matrixData;
}

Expression::Type ExpressionMatrix::type() const {
  return Type::ExpressionMatrix;
}

Expression * ExpressionMatrix::clone() const {
  return new ExpressionMatrix(m_matrixData->operands(), numberOfOperands(), numberOfRows(), numberOfColumns(), true);
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

template<typename T>
Evaluation<T> * ExpressionMatrix::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Complex<T> * operands = new Complex<T>[numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    Evaluation<T> * operandEvaluation = operand(i)->evaluate<T>(context, angleUnit);
    if (operandEvaluation->numberOfOperands() != 1) {
      operands[i] = Complex<T>::Float(NAN);
    } else {
      operands[i] = *(operandEvaluation->complexOperand(0));
    }
    delete operandEvaluation;
  }
  Evaluation<T> * matrix = new ComplexMatrix<T>(operands, numberOfRows(), numberOfColumns());
  delete[] operands;
  return matrix;
}

}
