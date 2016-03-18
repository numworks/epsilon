#include <poincare/subtraction.h>
#include "layout/horizontal_layout.h"

Substraction::Substraction(Expression * first_operand, Expression * second_operand) {
  m_left = first_operand;
  m_right = second_operand;
}

Substraction::~Substraction() {
  delete m_left;
  delete m_right;
}

float Substraction::approximate(Context& context) {
  return m_left->approximate(context) - m_right->approximate(context);
}

ExpressionLayout * Substraction::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_left, '-', m_right);
}
