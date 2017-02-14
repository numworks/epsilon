extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include "layout/matrix_layout.h"
#include <math.h>
#include <string.h>

namespace Poincare {

Matrix::Matrix(MatrixData * matrixData) :
  m_matrixData(matrixData)
{
}

Matrix::~Matrix() {
  delete m_matrixData;
}

Matrix::Matrix(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands)
{
  m_matrixData = new MatrixData(newOperands, numberOfOperands, numberOfColumns, numberOfRows, cloneOperands);
}

Complex * Matrix::defaultExpression() {
  static Complex * defaultExpression = new Complex(0.0f);
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

ExpressionLayout * Matrix::privateCreateLayout(FloatDisplayMode floatDisplayMode) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(numberOfOperands()*sizeof(ExpressionLayout *));
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout(floatDisplayMode);
  }
  return new MatrixLayout(childrenLayouts, numberOfRows(), numberOfColumns());
}

float Matrix::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * Matrix::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * operands[numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    operands[i] = operand(i)->evaluate(context, angleUnit);
    assert(operands[i]->type() == Type::Matrix || operands[i]->type() == Type::Complex);
    if (operands[i]->type() == Type::Matrix) {
      delete operands[i];
      operands[i] = new Complex(NAN);
      continue;
    }
  }
  return new Matrix(new MatrixData(operands, numberOfOperands(), numberOfColumns(), numberOfRows(), false));
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

}
