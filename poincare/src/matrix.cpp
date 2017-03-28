extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include "layout/grid_layout.h"
#include "layout/bracket_layout.h"
#include <math.h>
#include <float.h>
#include <string.h>

namespace Poincare {

Matrix::Matrix(MatrixData * matrixData) :
  m_matrixData(matrixData)
{
}

Matrix::~Matrix() {
  delete m_matrixData;
}

bool Matrix::hasValidNumberOfArguments() const {
  for (int i = 0; i < numberOfOperands(); i++) {
    if (!operand(i)->hasValidNumberOfArguments()) {
      return false;
    }
  }
  return true;
}

Matrix::Matrix(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands)
{
  m_matrixData = new MatrixData(newOperands, numberOfOperands, numberOfColumns, numberOfRows, cloneOperands);
}

Complex * Matrix::defaultExpression() {
  static Complex * defaultExpression = new Complex(Complex::Float(0.0f));
  return defaultExpression;
}

int Matrix::numberOfOperands() const {
  return m_matrixData->numberOfRows() * m_matrixData->numberOfColumns();
}

const Expression * Matrix::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  return m_matrixData->operands()[i];
}

Expression * Matrix::clone() const {
  return this->cloneWithDifferentOperands(m_matrixData->operands(), numberOfOperands(), true);
}

ExpressionLayout * Matrix::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(numberOfOperands()*sizeof(ExpressionLayout *));
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * layout = new BracketLayout(new GridLayout(childrenLayouts, numberOfRows(), numberOfColumns()));
  free(childrenLayouts);
  return layout;
}

float Matrix::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * Matrix::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression ** operands = (Expression **)malloc(numberOfOperands()*sizeof(Expression *));
  for (int i = 0; i < numberOfOperands(); i++) {
    operands[i] = operand(i)->evaluate(context, angleUnit);
    assert(operands[i]->type() == Type::Matrix || operands[i]->type() == Type::Complex);
    if (operands[i]->type() == Type::Matrix) {
      delete operands[i];
      operands[i] = new Complex(Complex::Float(NAN));
      continue;
    }
  }
  Expression * matrix = new Matrix(new MatrixData(operands, numberOfOperands(), numberOfColumns(), numberOfRows(), false));
  free(operands);
  return matrix;
}

Expression::Type Matrix::type() const {
  return Type::Matrix;
}

Expression * Matrix::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new Matrix(newOperands, numberOfOperands, numberOfColumns(), numberOfRows(), cloneOperands);
}

int Matrix::numberOfRows() const {
  return m_matrixData->numberOfRows();
}

int Matrix::numberOfColumns() const {
  return m_matrixData->numberOfColumns();
}

int Matrix::writeTextInBuffer(char * buffer, int bufferSize) {
  buffer[bufferSize] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize) {
    return 0;
  }
  buffer[currentChar++] = '[';
  if (currentChar >= bufferSize) {
    return currentChar;
  }
  for (int i = 0; i < m_matrixData->numberOfRows(); i++) {
    buffer[currentChar++] = '[';
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    currentChar += m_matrixData->operands()[i*m_matrixData->numberOfColumns()]->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    for (int j = 1; j < numberOfColumns(); j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize) {
        return currentChar;
      }
      currentChar += m_matrixData->operands()[i*m_matrixData->numberOfColumns()+j]->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
      if (currentChar >= bufferSize) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    buffer[currentChar++] = ']';
    if (currentChar >= bufferSize) {
      return currentChar;
    }
  }
  buffer[currentChar++] = ']';
  if (currentChar >= bufferSize) {
    return currentChar;
  }
  buffer[currentChar] = 0;
  return currentChar;
}

Expression * Matrix::createDimensionMatrix(Context& context, AngleUnit angleUnit) const {
  Expression * operands[2];
  operands[0] = new Complex(Complex::Float(numberOfRows()));
  operands[1] = new Complex(Complex::Float(numberOfColumns()));
  return new Matrix(new MatrixData(operands, 2, 2, 1, false));
}

float Matrix::trace(Context& context, AngleUnit angleUnit) const {
  if (numberOfColumns() != numberOfRows()) {
    return NAN;
  }
  int dim = numberOfRows();
  float trace = 0.0f;
  for (int i = 0; i < dim; i++) {
    trace += m_matrixData->operands()[i*dim+i]->approximate(context, angleUnit);
  }
  return trace;
}

float Matrix::determinant(Context& context, AngleUnit angleUnit) const {
  if (numberOfColumns() != numberOfRows()) {
    return NAN;
  }
  int dim = numberOfRows();
  float ** tempMat = (float **)malloc(dim*sizeof(float*));
  for (int i = 0; i < dim; i++) {
    tempMat[i] = (float *)malloc(dim*sizeof(float));
  }
  float det = 1.0f;
  /* Copy the matrix */
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      tempMat[i][j] = m_matrixData->operands()[i*dim+j]->approximate(context, angleUnit);
    }
  }

/* Main Loop: Gauss pivot */
  for (int i = 0; i < dim-1; i++) {
  /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (fabsf(tempMat[rowWithPivot][i]) < fabsf(tempMat[row][i])) {
        rowWithPivot = row;
      }
    }
    float valuePivot = tempMat[rowWithPivot][i];
  /* if the pivot is null, det = 0. */
    if (fabsf(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(tempMat[i]);
      }
      free(tempMat);
      return 0.0f;
    }
/* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < dim; col++) {
        float temp = tempMat[i][col];
        tempMat[i][col] = tempMat[rowWithPivot][col];
        tempMat[rowWithPivot][col] = temp;
      }
      det *= -1;
    }
    det *= valuePivot;
/* Set to 0 all A[][i] by linear combination */
    for (int row = i+1; row < dim; row++) {
      float factor = tempMat[row][i]/valuePivot;
      for (int col = i; col < dim; col++) {
        tempMat[row][col] -= factor*tempMat[i][col];
      }
    }
  }
  det *= tempMat[dim-1][dim-1];
  for (int i = 0; i < dim; i++) {
    free(tempMat[i]);
  }
  free(tempMat);
  return det;
}

Expression * Matrix::createInverse(Context& context, AngleUnit angleUnit) const {
  if (numberOfColumns() != numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  float ** inv = (float **)malloc(dim*sizeof(float*));
  for (int i = 0; i < dim; i++) {
    inv[i] = (float *)malloc(2*dim*sizeof(float));
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      inv[i][j] = m_matrixData->operands()[i*dim+j]->approximate(context, angleUnit);
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
      if (fabsf(inv[rowWithPivot][i]) < fabsf(inv[row][i])) {
        rowWithPivot = row;
      }
    }
    float valuePivot = inv[rowWithPivot][i];
  /* if the pivot is null, the matrix in not invertible. */
    if (fabsf(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(inv[i]);
      }
      free(inv);
      return new Complex(Complex::Float(NAN));
    }
  /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < 2*dim; col++) {
        float temp = inv[i][col];
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
      float factor = inv[row][i];
      for (int col = 0; col < 2*dim; col++) {
        inv[row][col] -= factor*inv[i][col];
      }
    }
  }
  Expression ** operands = (Expression **)malloc(numberOfOperands()*sizeof(Expression *));
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*dim+j] = new Complex(Complex::Float(inv[i][j+dim]));
    }
  }
  for (int i = 0; i < dim; i++) {
    free(inv[i]);
  }
  free(inv);
  Expression * matrix = new Matrix(new MatrixData(operands, numberOfOperands(), numberOfColumns(), numberOfRows(), false));
  free(operands);
  return matrix;
}

Expression * Matrix::createTranspose(Context& context, AngleUnit angleUnit) const {
  Expression ** operands = (Expression **)malloc(numberOfOperands()*sizeof(Expression *));
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = m_matrixData->operands()[i*numberOfColumns()+j]->clone();
    }
  }
  Expression * matrix = new Matrix(new MatrixData(operands, numberOfOperands(), numberOfRows(), numberOfColumns(), false));
  free(operands);
  return matrix;
}

}
