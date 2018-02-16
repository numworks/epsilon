extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/addition.h>
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
  ExpressionLayout * layout = new BracketLayout(new GridLayout(childrenLayouts, numberOfRows(), numberOfColumns()));
  delete [] childrenLayouts;
  return layout;
}

template<typename T>
Complex<T> * Matrix::createTrace() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int dim = numberOfRows();
  Complex<T> c = Complex<T>::Float(0);
  for (int i = 0; i < dim; i++) {
    assert(operand(i*dim+i)->type() == Type::Complex);
    c = Addition::compute(c, *(static_cast<const Complex<T> *>(operand(i*dim+i))));
  }
  return new Complex<T>(c);
}

// TODO: 1. implement determinant/inverse for complex matrix
// TODO: 2. implement determinant/inverse for any expression (do not evaluate first)
template<typename T>
Complex<T> * Matrix::createDeterminant() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int dim = numberOfRows();
  T ** tempMat = new T*[dim];
  for (int i = 0; i < dim; i++) {
    tempMat[i] = new T[dim];
  }
  T det = 1;
  /* Copy the matrix */
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      const Expression * op = operand(i*dim+j);
      assert(op->type() == Type::Complex);
      tempMat[i][j] = static_cast<const Complex<T> *>(op)->toScalar(); // TODO: keep complex
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
    T valuePivot = tempMat[rowWithPivot][i];
    /* if the pivot is null, det = 0. */
    if (std::fabs(valuePivot) <= (sizeof(T) == sizeof(float) ? FLT_EPSILON : DBL_EPSILON)) {
      for (int i = 0; i < dim; i++) {
        delete[] tempMat[i];
      }
      delete[] tempMat;
      return new Complex<T>(Complex<T>::Float(0.0));
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < dim; col++) {
        T temp = tempMat[i][col];
        tempMat[i][col] = tempMat[rowWithPivot][col];
        tempMat[rowWithPivot][col] = temp;
      }
      det *= -1;
    }
    det *= valuePivot;
    /* Set to 0 all A[][i] by linear combination */
    for (int row = i+1; row < dim; row++) {
      T factor = tempMat[row][i]/valuePivot;
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
  return new Complex<T>(Complex<T>::Float(det));
}

template<typename T>
Matrix * Matrix::createInverse() const {
  if (numberOfRows() != numberOfColumns()) {
    return nullptr;
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  T ** inv = new T*[dim];
  for (int i = 0; i < dim; i++) {
    inv[i] = new T [2*dim];
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      const Expression * op = operand(i*dim+j);
      assert(op->type() == Type::Complex);
      inv[i][j] = static_cast<const Complex<T> *>(op)->toScalar(); // TODO: keep complex
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
    T valuePivot = inv[rowWithPivot][i];
    /* if the pivot is null, the matrix in not invertible. */
    if (std::fabs(valuePivot) <= (sizeof(T) == sizeof(float) ? FLT_EPSILON : DBL_EPSILON)) {
      for (int i = 0; i < dim; i++) {
        delete[] inv[i];
      }
      delete[] inv;
      return nullptr;
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < 2*dim; col++) {
        T temp = inv[i][col];
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
      T factor = inv[row][i];
      for (int col = 0; col < 2*dim; col++) {
        inv[row][col] -= factor*inv[i][col];
      }
    }
  }
  const Expression ** operands = new const Expression * [numberOfOperands()];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*dim+j] = new Complex<T>(Complex<T>::Float(inv[i][j+dim]));
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
Matrix * Matrix::createApproximateIdentity(int dim) {
  Expression ** operands = new Expression * [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = new Complex<T>(Complex<T>::Float(1));
      } else {
        operands[i*dim+j] = new Complex<T>(Complex<T>::Float(0));
      }
    }
  }
  Matrix * matrix = new Matrix(operands, dim, dim, false);
  delete [] operands;
  return matrix;
}

template<typename T>
Expression * Matrix::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression ** operands = new Expression * [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    Expression * operandEvaluation = operand(i)->approximate<T>(context, angleUnit);
    if (operandEvaluation->type() != Type::Complex) {
      operands[i] = new Complex<T>(Complex<T>::Float(NAN));
      delete operandEvaluation;
    } else {
      operands[i] = operandEvaluation;
    }
  }
  Expression * matrix = new Matrix(operands, numberOfRows(), numberOfColumns(), false);
  delete[] operands;
  return matrix;
}

template Matrix* Matrix::createApproximateIdentity<float>(int);
template Matrix* Matrix::createApproximateIdentity<double>(int);
template Complex<float>* Matrix::createTrace<float>() const;
template Complex<double>* Matrix::createTrace<double>() const;
template Matrix* Matrix::createInverse<float>() const;
template Matrix* Matrix::createInverse<double>() const;
template Complex<float>* Matrix::createDeterminant<float>() const;
template Complex<double>* Matrix::createDeterminant<double>() const;
}
