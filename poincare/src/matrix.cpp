extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/addition.h>
#include <poincare/decimal.h>
#include <poincare/undefined.h>
#include "layout/matrix_layout.h"
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

Matrix::Matrix(MatrixData * matrixData) :
  DynamicHierarchy()
{
  assert(matrixData != nullptr);
  m_numberOfOperands = matrixData->numberOfRows()*matrixData->numberOfColumns();
  m_numberOfRows = matrixData->numberOfRows();
  matrixData->pilferOperands(&m_operands);
  for (int i = 0; i < m_numberOfOperands; i++) {
    const_cast<Expression *>(m_operands[i])->setParent(this);
  }
}

Matrix::Matrix(const Expression * const * operands, int numberOfRows, int numberOfColumns, bool cloneOperands) :
  DynamicHierarchy(operands, numberOfRows*numberOfColumns, cloneOperands),
  m_numberOfRows(numberOfRows)
{
}

int Matrix::numberOfRows() const {
  return m_numberOfRows;
}

int Matrix::numberOfColumns() const {
  return numberOfOperands()/m_numberOfRows;
}

Expression::Type Matrix::type() const {
  return Type::Matrix;
}

Expression * Matrix::clone() const {
  return new Matrix(m_operands, numberOfRows(), numberOfColumns(), true);
}

int Matrix::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) {
    return 0;
  }
  buffer[currentChar++] = '[';
  if (currentChar >= bufferSize-1) {
    return currentChar;
  }
  for (int i = 0; i < numberOfRows(); i++) {
    buffer[currentChar++] = '[';
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    currentChar += operand(i*numberOfColumns())->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, numberOfSignificantDigits);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    for (int j = 1; j < numberOfColumns(); j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
      currentChar += operand(i*numberOfColumns()+j)->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, numberOfSignificantDigits);
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    buffer[currentChar++] = ']';
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
  }
  buffer[currentChar++] = ']';
  buffer[currentChar] = 0;
  return currentChar;
}

int Matrix::polynomialDegree(char symbolName) const {
  return -1;
}

ExpressionLayout * Matrix::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** childrenLayouts = new ExpressionLayout * [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * layout = new MatrixLayout(childrenLayouts, numberOfRows(), numberOfColumns(), false);
  delete [] childrenLayouts;
  return layout;
}

Matrix * Matrix::createTranspose() const {
  const Expression ** operands = new const Expression * [numberOfOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = operand(i*numberOfColumns()+j);
    }
  }
  // Intentionally swapping dimensions for transpose
  Matrix * matrix = new Matrix(operands, numberOfColumns(), numberOfRows(), true);
  delete[] operands;
  return matrix;
}

Matrix * Matrix::createIdentity(int dim) {
  Expression ** operands = new Expression * [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = new Rational(1);
      } else {
        operands[i*dim+j] = new Rational(0);
      }
    }
  }
  Matrix * matrix = new Matrix(operands, dim, dim, false);
  delete [] operands;
  return matrix;
}

template<typename T>
Evaluation<T> * Matrix::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  std::complex<T> * operands = new std::complex<T> [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    Evaluation<T> * operandEvaluation = operand(i)->privateApproximate(T(), context, angleUnit);
    if (operandEvaluation->type() != Evaluation<T>::Type::Complex) {
      operands[i] = Complex<T>::Undefined();
    } else {
      std::complex<T> * c = static_cast<Complex<T> *>(operandEvaluation);
      operands[i] = *c;
    }
    delete operandEvaluation;
  }
  MatrixComplex<T> * matrix = new MatrixComplex<T>(operands, numberOfRows(), numberOfColumns());
  delete[] operands;
  return matrix;
}

}
