extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/decimal.h>
#include <poincare/undefined.h>
#include "layout/grid_layout.h"
#include "layout/bracket_layout.h"
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

int Matrix::writeTextInBuffer(char * buffer, int bufferSize) const {
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
    currentChar += operand(i*numberOfColumns())->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    for (int j = 1; j < numberOfColumns(); j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
      currentChar += operand(i*numberOfColumns()+j)->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
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

ExpressionLayout * Matrix::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** childrenLayouts = new ExpressionLayout * [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * layout = new BracketLayout(new GridLayout(childrenLayouts, numberOfRows(), numberOfColumns()));
  delete [] childrenLayouts;
  return layout;
}

// TODO: 1. implement determinant/inverse for complex matrix
// TODO: 2. implement determinant/inverse for any expression (do not evaluate first)
Expression * Matrix::createDeterminant() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Undefined();
  }
  int dim = numberOfRows();
  double ** tempMat = new double*[dim];
  for (int i = 0; i < dim; i++) {
    tempMat[i] = new double[dim];
  }
  double det = 1;
  /* Copy the matrix */
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      const Expression * op = operand(i*dim+j);
      assert(op->type() == Type::Complex);
      tempMat[i][j] = static_cast<const Complex<double> *>(op)->toScalar(); // TODO: keep complex
    }
  }

  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim-1; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (std::fabs(tempMat[rowWithPivot][i]) < std::fabs(tempMat[row][i])) {
        rowWithPivot = row;
      }
    }
    double valuePivot = tempMat[rowWithPivot][i];
    /* if the pivot is null, det = 0. */
    if (std::fabs(valuePivot) <= DBL_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(tempMat[i]);
      }
      free(tempMat);
      return new Decimal(0.0);
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < dim; col++) {
        double temp = tempMat[i][col];
        tempMat[i][col] = tempMat[rowWithPivot][col];
        tempMat[rowWithPivot][col] = temp;
      }
      det *= -1;
    }
    det *= valuePivot;
    /* Set to 0 all A[][i] by linear combination */
    for (int row = i+1; row < dim; row++) {
      double factor = tempMat[row][i]/valuePivot;
      for (int col = i; col < dim; col++) {
        tempMat[row][col] -= factor*tempMat[i][col];
      }
    }
  }
  det *= tempMat[dim-1][dim-1];
  for (int i = 0; i < dim; i++) {
    delete[] tempMat[i];
  }
  delete[] tempMat;
  return new Decimal(det);
}

Expression * Matrix::createInverse() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Undefined();
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  double ** inv = new double*[dim];
  for (int i = 0; i < dim; i++) {
    inv[i] = new double [2*dim];
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      const Expression * op = operand(i*dim+j);
      assert(op->type() == Type::Complex);
      inv[i][j] = static_cast<const Complex<double> *>(op)->toScalar(); // TODO: keep complex
    }
    for (int j = dim; j < 2*dim; j++) {
      inv[i][j] = (i+dim == j);
    }
  }
  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (std::fabs(inv[rowWithPivot][i]) < std::fabs(inv[row][i])) {
        rowWithPivot = row;
      }
    }
    double valuePivot = inv[rowWithPivot][i];
    /* if the pivot is null, the matrix in not invertible. */
    if (std::fabs(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(inv[i]);
      }
      free(inv);
      return new Undefined();
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < 2*dim; col++) {
        double temp = inv[i][col];
        inv[i][col] = inv[rowWithPivot][col];
        inv[rowWithPivot][col] = temp;
      }
    }
    /* A[pivot][] = A[pivot][]/valuePivot */
    for (int col = 0; col < 2*dim; col++) {
      inv[i][col] /= valuePivot;
    }
    /* Set to 0 all A[][row] by linear combination */
    for (int row = 0; row < dim; row++) {
      if (row == i) {
        continue;
      }
      double factor = inv[row][i];
      for (int col = 0; col < 2*dim; col++) {
        inv[row][col] -= factor*inv[i][col];
      }
    }
  }
  const Expression ** operands = new const Expression * [numberOfOperands()];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*dim+j] = new Decimal(inv[i][j+dim]);
    }
  }
  for (int i = 0; i < dim; i++) {
    delete[] inv[i];
  }
  delete[] inv;
  // Intentionally swapping dimensions for inverse, although it doesn't make a difference because it is square
  Matrix * matrix = new Matrix(operands, numberOfColumns(), numberOfRows(), false);
  delete[] operands;
  return matrix;
}

Matrix * Matrix::createTranspose() const {
  const Expression ** operands = new const Expression * [numberOfOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = operand(i*numberOfColumns()+j);
    }
  }
  // Intentionally swapping dimensions for transpose
  Matrix * matrix = new Matrix(operands, numberOfColumns(), numberOfRows());
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
  Matrix * matrix = new Matrix(operands, dim, dim);
  delete [] operands;
  return matrix;
}

}
