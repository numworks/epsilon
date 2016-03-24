#include <poincare/fraction.h>
#include <string.h>
#include "layout/fraction_layout.h"

Expression * Fraction::clone() {
  return new Fraction(m_operands, true);
}

ExpressionLayout * Fraction::createLayout(ExpressionLayout * parent) {
  return new FractionLayout(parent, m_operands[0], m_operands[1]);
}

float Fraction::approximate(Context& context) {
  // TODO: handle division by zero
  return m_operands[0]->approximate(context)/m_operands[1]->approximate(context);
}

Expression::Type Fraction::type() {
  return Expression::Type::Fraction;
}
