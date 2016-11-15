extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/parenthesis.h>
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"

Parenthesis::Parenthesis(Expression * operand, bool cloneOperands) {
  assert(operand != nullptr);
  if (cloneOperands) {
    m_operand = operand->clone();
  } else {
    m_operand = operand;
  }
}

Parenthesis::~Parenthesis() {
  delete m_operand;
}

int Parenthesis::numberOfOperands() const {
  return 1;
}

const Expression * Parenthesis::operand(int i) const {
  assert(i == 0);
  return m_operand;
}

Expression * Parenthesis::clone() const {
  return this->cloneWithDifferentOperands((Expression**) &m_operand, 1, true);
}

ExpressionLayout * Parenthesis::createLayout() const {
  // TODO: create a parenthesis layout to adjust parenthesis sizes to the operand
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new StringLayout("(", 1);
  childrenLayouts[1] = m_operand->createLayout();
  childrenLayouts[2] = new StringLayout(")", 1);
  return new HorizontalLayout(childrenLayouts, 3);
}

float Parenthesis::approximate(Context& context) const {
  return m_operand->approximate(context);
}

Expression::Type Parenthesis::type() const {
  return Expression::Type::Parenthesis;
}

Expression * Parenthesis::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  return new Parenthesis(newOperands[0], cloneOperands);
}
