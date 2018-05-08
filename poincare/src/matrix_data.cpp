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
  m_operands = new const Expression *[m_numberOfColumns];
  for (int i = 0; i < m_numberOfColumns; i++) {
    if (clone) {
      m_operands[i] = listData->operand(i)->clone();
    } else {
      m_operands[i] = listData->operand(i);
    }
  }
}

MatrixData::~MatrixData() {
  if (m_operands != nullptr) {
    for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
      delete m_operands[i];
    }
    delete[] m_operands;
  }
}

void MatrixData::pushListData(ListData * listData, bool clone) {
  const Expression ** newOperands = new const Expression * [(m_numberOfRows+1)*m_numberOfColumns];
  assert(listData->numberOfOperands() == m_numberOfColumns);
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    newOperands[i] = m_operands[i];
  }
  for (int i = 0; i < m_numberOfColumns; i++) {
    if (clone) {
      newOperands[m_numberOfRows*m_numberOfColumns+i] = listData->operand(i)->clone();
    } else {
      newOperands[m_numberOfRows*m_numberOfColumns+i] = listData->operand(i);
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

void MatrixData::pilferOperands(const Expression *** newStorage) {
  *newStorage = m_operands;
  m_operands = nullptr;
}

}
