#include <poincare/subtraction.h>
#include "layout/horizontal_layout.h"

Expression * Subtraction::clone() {
  return new Subtraction(m_operands, true);
}

float Subtraction::approximate(Context& context) {
  return m_operands[0]->approximate(context) - m_operands[1]->approximate(context);
}

Expression::Type Subtraction::type() {
  return Expression::Type::Subtraction;
}

ExpressionLayout * Subtraction::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_operands, 2, '-');
}
