#include <poincare/opposite.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

Opposite::Opposite(Expression * operand, bool cloneOperands) {
  assert(operand != nullptr);
  if (cloneOperands) {
    m_operand = operand->clone();
  } else {
    m_operand = operand;
  }
}

Opposite::~Opposite() {
  delete m_operand;
}

const Expression * Opposite::operand(int i) const {
  assert(i == 0);
  return m_operand;
}

int Opposite::numberOfOperands() const {
  return 1;
}

Expression * Opposite::clone() const {
  return this->cloneWithDifferentOperands((Expression**)&m_operand, 1, true);
}

Expression * Opposite::evaluate(Context& context) const {
  Expression * operandEvalutation = m_operand->evaluate(context);
  if (operandEvalutation == nullptr) {
    return nullptr;
  }
  Expression * result = nullptr;
  if (operandEvalutation->type() == Type::Float) {
    result = new Float(this->approximate(context));
  }
  if (operandEvalutation->type() == Type::Matrix) {
    result = evaluateOnMatrix((Matrix *)operandEvalutation, context);
  }
  delete operandEvalutation;
  return result;
}

ExpressionLayout * Opposite::createLayout(DisplayMode displayMode) const {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  char string[2] = {'-', '\0'};
  children_layouts[0] = new StringLayout(string, 1);
  children_layouts[1] = m_operand->type() == Type::Opposite ? new ParenthesisLayout(m_operand->createLayout(displayMode)) : m_operand->createLayout(displayMode);
  return new HorizontalLayout(children_layouts, 2);
}

float Opposite::approximate(Context& context) const {
  return -m_operand->approximate(context);
}

Expression::Type Opposite::type() const {
  return Expression::Type::Opposite;
}

Expression * Opposite::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  return new Opposite(newOperands[0], cloneOperands);
}

Expression * Opposite::evaluateOnMatrix(Matrix * m, Context& context) const {
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    operands[i] = new Float(- m->operand(i)->approximate(context));
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}
