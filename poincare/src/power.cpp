#include <poincare/power.h>
#include <math.h>
#include "layout/exponent_layout.h"

Expression * Power::clone() {
  return new Power(m_operands, true);
}

float Power::approximate(Context& context) {
  return powf(m_operands[0]->approximate(context), m_operands[1]->approximate(context));
}

Expression::Type Power::type() {
  return Expression::Type::Power;
}

ExpressionLayout * Power::createLayout(ExpressionLayout * parent) {
  return new ExponentLayout(parent, m_operands[0], m_operands[1]);
}
