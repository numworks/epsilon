extern "C" {
#include <assert.h>
#include <string.h>
}

#include <poincare/fraction.h>
#include "layout/fraction_layout.h"

Expression * Fraction::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Fraction(newOperands, cloneOperands);
}

ExpressionLayout * Fraction::createLayout() const {
  return new FractionLayout(m_operands[0]->createLayout(), m_operands[1]->createLayout());
}

float Fraction::approximate(Context& context) const {
  // TODO: handle division by zero
  return m_operands[0]->approximate(context)/m_operands[1]->approximate(context);
}

Expression::Type Fraction::type() const {
  return Type::Fraction;
}

Expression * Fraction::evaluateOnMatrixAndFloat(Matrix * m, Float * a, Context& context) const {
  Expression * operands[m->numberOfRows() * m->numberOfColumns()];
  for (int i = 0; i < m->numberOfRows() * m->numberOfColumns(); i++) {
    operands[i] = new Float(m->operand(i)->approximate(context)/a->approximate(context));
  }
  Expression * result = new Matrix(operands, m->numberOfRows() * m->numberOfColumns(), m->numberOfColumns(), m->numberOfRows(), false);
  return result;
}

Expression * Fraction::evaluateOnMatrices(Matrix * m, Matrix * n, Context& context) const {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  /* TODO: implement matrix fraction
  if (n->det() == 0) {
    return new Float(NAN);
  }
  result = new Product(m, n->inv());
  return result;*/
  return nullptr;
}
