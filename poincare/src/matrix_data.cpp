extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix_data.h>
#include <poincare/complex.h>

namespace Poincare {

MatrixData::MatrixData(ListData * listData, bool clone) :
  m_numberOfRows(1),
  m_numberOfColumns(0)
{
  assert(listData != nullptr);
  m_numberOfColumns = listData->numberOfOperands();
  m_operands = new Expression *[m_numberOfColumns];
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
  m_operands = new Expression *[m_numberOfRows*m_numberOfColumns];
  for (int i = 0; i < m_numberOfColumns*m_numberOfRows; i++) {
    if (cloneOperands) {
      m_operands[i] = i < numberOfOperands ? newOperands[i]->clone() : defaultExpression();
    } else {
      m_operands[i] = i < numberOfOperands ? newOperands[i] : defaultExpression();
    }
  }
}

Complex<double> * MatrixData::defaultExpression() {
  static Complex<double> * defaultExpression = new Complex<double>(Complex<double>::Float(0.0));
  return defaultExpression;
}

MatrixData::~MatrixData() {
  for (int i=0; i<m_numberOfColumns*m_numberOfRows; i++) {
    // Warning: avoid deleting the defaultExpression
    if (m_operands[i] != defaultExpression()) {
      delete m_operands[i];
    }
  }
  delete[] m_operands;
}

void MatrixData::pushListData(ListData * listData, bool clone) {
  Expression ** newOperands = new Expression * [(m_numberOfRows+1)*m_numberOfColumns];
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
  delete[] m_operands;
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
