extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <math.h>
#include <poincare/float.h>
#include <poincare/list.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

List::List(Expression * operand, bool cloneOperands) :
  m_numberOfOperands(1),
  m_operands((Expression **)malloc(sizeof(Expression *)))
{
  assert(operand != nullptr);
  if (cloneOperands) {
    m_operands[0] = operand->clone();
  } else {
    m_operands[0] = operand;
  }
}

List::List(Expression ** operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfOperands(numberOfOperands),
  m_operands((Expression **)malloc(numberOfOperands*sizeof(Expression *)))
{
  assert(operands != nullptr);
  for (int i = 0; i < numberOfOperands; i++) {
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
  }
}

List::~List() {
  for (int i=0; i<m_numberOfOperands; i++) {
    delete m_operands[i];
  }
  free(m_operands);
}

void List::pushExpression(Expression * operand, bool cloneOperand) {
  Expression ** newOperands = (Expression **)malloc((m_numberOfOperands+1)*sizeof(Expression *));
  for (int i=0; i<m_numberOfOperands; i++) {
    newOperands[i] = m_operands[i];
  }
  free(m_operands);
  if (cloneOperand) {
    newOperands[m_numberOfOperands] = operand->clone();
  } else {
    newOperands[m_numberOfOperands] = operand;
  }
  m_operands = newOperands;
  m_numberOfOperands++;
}

int List::numberOfOperands() const {
  return m_numberOfOperands;
}

const Expression * List::operand(int i) const {
  assert(i >= 0);
  assert(i < m_numberOfOperands);
  return m_operands[i];
}

Expression * List::clone() const {
  return this->cloneWithDifferentOperands(m_operands, m_numberOfOperands, true);
}

ExpressionLayout * List::createLayout() const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc((2*m_numberOfOperands-1)*sizeof(ExpressionLayout *));
  for (int i=0; i<(m_numberOfOperands-1); i++) {
    childrenLayouts[2*i] = m_operands[i]->createLayout();
    childrenLayouts[2*i+1] = new StringLayout(",", 1);
  }
  childrenLayouts[2*m_numberOfOperands-2] = m_operands[m_numberOfOperands-1]->createLayout();
  return new HorizontalLayout(childrenLayouts, 2*m_numberOfOperands-1);
}

float List::approximate(Context& context) const {
  return NAN;
}

Expression * List::createEvaluation(Context& context) const {
  Expression * operands[m_numberOfOperands];
  for (int i = 0; i < m_numberOfOperands; i++) {
    operands[i] = new Float(m_operands[i]->approximate(context));
  }
  return new List(operands, m_numberOfOperands, false);
}

Expression::Type List::type() const {
  return Expression::Type::List;
}

Expression * List::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new List(newOperands, numberOfOperands, cloneOperands);
}
