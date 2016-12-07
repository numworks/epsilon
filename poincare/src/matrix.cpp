extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

Matrix::Matrix(List * list, bool cloneOperands) :
  m_numberOfRows(1),
  m_numberOfColumns(0)
{
  assert(list != nullptr);
  m_numberOfColumns = list->numberOfOperands();
  m_operands = (Expression **)malloc(m_numberOfColumns*sizeof(Expression *));
  for (int i = 0; i < m_numberOfColumns; i++) {
    if (cloneOperands) {
      m_operands[i] = list->operand(i)->clone();
    } else {
      m_operands[i] = (Expression *)list->operand(i);
    }
  }
}


Integer * Matrix::defaultExpression() {
  static Integer * defaultExpression = new Integer(0);
  return defaultExpression;
}

Matrix::Matrix(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands) :
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

Matrix::~Matrix() {
  for (int i=0; i<m_numberOfColumns*m_numberOfRows; i++) {
    // Warning: avoid deleting the defaultExpression
    if (m_operands[i] != defaultExpression()) {
      delete m_operands[i];
    }
  }
  free(m_operands);
}

void Matrix::pushList(List * list, bool cloneOperands) {
  Expression ** newOperands = (Expression **)malloc(((m_numberOfRows+1)*m_numberOfColumns)*sizeof(Expression *));
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    newOperands[i] = m_operands[i];
  }
  for (int i = 0; i < m_numberOfColumns; i++) {
    if (cloneOperands) {
      newOperands[m_numberOfRows*m_numberOfColumns+i] = i < list->numberOfOperands() ? list->operand(i)->clone() : defaultExpression();
    } else {
      int max = list->numberOfOperands();
      newOperands[m_numberOfRows*m_numberOfColumns+i] = i < max ? (Expression *)list->operand(i) : defaultExpression();
    }
  }
  free(m_operands);
  m_operands = newOperands;
  m_numberOfRows++;
}

int Matrix::numberOfOperands() const {
  return m_numberOfRows*m_numberOfColumns;
}

const Expression * Matrix::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  return m_operands[i];
}

Expression * Matrix::clone() const {
  return this->cloneWithDifferentOperands(m_operands, m_numberOfRows*m_numberOfColumns, true);
}

ExpressionLayout * Matrix::createLayout() const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc((2+m_numberOfRows*(1+2*m_numberOfColumns))*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new StringLayout("[", 1);
  for (int i = 0; i < m_numberOfRows; i++) {
    childrenLayouts[i*(2*m_numberOfColumns+1)+1] = new StringLayout("[", 1);
    for (int j = 0; j < (m_numberOfColumns - 1); j++) {
      childrenLayouts[i*(2*m_numberOfColumns+1)+2*(j+1)] = m_operands[i*m_numberOfColumns+j]->createLayout();
      childrenLayouts[i*(2*m_numberOfColumns+1)+2*(j+1)+1] = new StringLayout(",", 1);
    }
    childrenLayouts[(i+1)*(2*m_numberOfColumns+1)-1] = m_operands[(i+1)*m_numberOfColumns-1]->createLayout();
    childrenLayouts[(i+1)*(2*m_numberOfColumns+1)] = new StringLayout("]", 1);
  }
  childrenLayouts[m_numberOfRows*(2*m_numberOfColumns+1)+1] = new StringLayout("]", 1);
  return new HorizontalLayout(childrenLayouts, 2+m_numberOfRows*(1+2*m_numberOfColumns));
}

float Matrix::approximate(Context& context) const {
  return 0;
}

Expression::Type Matrix::type() const {
  return Expression::Type::Matrix;
}

Expression * Matrix::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new Matrix(newOperands, numberOfOperands, m_numberOfColumns, m_numberOfRows, cloneOperands);
}
