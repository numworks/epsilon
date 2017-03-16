extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix_data.h>
#include <poincare/integer.h>

namespace Poincare {

MatrixData::MatrixData(ListData * listData, bool clone) :
  m_numberOfRows(1),
  m_numberOfColumns(0)
{
  assert(listData != nullptr);
  m_numberOfColumns = listData->numberOfOperands();
  m_operands = (Expression **)malloc(m_numberOfColumns*sizeof(Expression *));
  for (int i = 0; i < m_numberOfColumns; i++) {
    if (clone) {
      m_operands[i] = (Expression *)listData->operand(i)->clone();
    } else {
      m_operands[i] = (Expression *)listData->operand(i);
    }
  }
}

MatrixData::MatrixData(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands) :
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  assert(newOperands != nullptr);
  m_operands = (Expression **)malloc(m_numberOfRows*m_numberOfColumns*sizeof(Expression *));
  for (int i = 0; i < m_numberOfColumns*m_numberOfRows; i++) {
    if (cloneOperands) {
      m_operands[i] = i < numberOfOperands ? newOperands[i]->clone() : defaultExpression();
    } else {
      m_operands[i] = i < numberOfOperands ? newOperands[i] : defaultExpression();
    }
  }
}

Complex * MatrixData::defaultExpression() {
  static Complex * defaultExpression = new Complex(Complex::Float(0.0f));
  return defaultExpression;
}

MatrixData::~MatrixData() {
  for (int i=0; i<m_numberOfColumns*m_numberOfRows; i++) {
    // Warning: avoid deleting the defaultExpression
    if (m_operands[i] != defaultExpression()) {
      delete m_operands[i];
    }
  }
  free(m_operands);
}

void MatrixData::pushListData(ListData * listData, bool clone) {
  Expression ** newOperands = (Expression **)malloc(((m_numberOfRows+1)*m_numberOfColumns)*sizeof(Expression *));
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    newOperands[i] = m_operands[i];
  }
  for (int i = 0; i < m_numberOfColumns; i++) {
    int max = listData->numberOfOperands();
    if (clone) {
      newOperands[m_numberOfRows*m_numberOfColumns+i] = i < max ? (Expression *)listData->operand(i)->clone() : defaultExpression();
    } else {
      newOperands[m_numberOfRows*m_numberOfColumns+i] = i < max ? (Expression *)listData->operand(i) : defaultExpression();
    }
  }
  free(m_operands);
  m_operands = newOperands;
  m_numberOfRows++;
}

int MatrixData::numberOfRows() {
  return m_numberOfRows;
}

int MatrixData::numberOfColumns() {
  return m_numberOfColumns;
}

Expression ** MatrixData::operands() const {
  return m_operands;
}

}
