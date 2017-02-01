extern "C" {
#include <assert.h>
#include <stdlib.h>
}

#include <poincare/multiplication.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"

Expression::Type Multiplication::type() const {
  return Expression::Type::Multiplication;
}

ExpressionLayout * Multiplication::createLayout(DisplayMode displayMode) const {
  ExpressionLayout** children_layouts = (ExpressionLayout **)malloc(3*sizeof(ExpressionLayout *));
  children_layouts[0] = m_operands[0]->createLayout(displayMode);
  children_layouts[1] = new StringLayout("*", 1);
  children_layouts[2] = m_operands[1]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[1]->createLayout(displayMode)) : m_operands[1]->createLayout(displayMode);
  return new HorizontalLayout(children_layouts, 3);
}

float Multiplication::approximate(Context& context, AngleUnit angleUnit) const {
  return m_operands[0]->approximate(context, angleUnit)*m_operands[1]->approximate(context, angleUnit);;
}

Expression * Multiplication::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Multiplication(newOperands, cloneOperands);
}

Expression * Multiplication::evaluateOnMatrixAndFloat(Matrix * m, Float * a, Context& context, AngleUnit angleUnit) const {
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    operands[i] = new Float(m->operand(i)->approximate(context, angleUnit)*a->approximate(context, angleUnit));
  }
  return new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
}

Expression * Multiplication::evaluateOnFloatAndMatrix(Float * a, Matrix * m, Context& context, AngleUnit angleUnit) const {
  return evaluateOnMatrixAndFloat(m, a, context, angleUnit);
}

Expression * Multiplication::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const {
  if (m->numberOfColumns() != n->numberOfRows()) {
    return nullptr;
  }
  Expression * operands[m->numberOfRows() * n->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows(); i++) {
    for (int j = 0; j < n->numberOfColumns(); j++) {
      float f = 0.0f;
      for (int k = 0; k < m->numberOfColumns(); k++) {
        f += m->operand(i*m->numberOfColumns()+k)->approximate(context, angleUnit) * n->operand(k*n->numberOfColumns()+j)->approximate(context, angleUnit);
      }
      operands[i*n->numberOfColumns()+j] = new Float(f);
    }
  }
  return new Matrix(operands, m->numberOfRows() * n->numberOfColumns(), m->numberOfRows(), n->numberOfColumns(), false);
}
