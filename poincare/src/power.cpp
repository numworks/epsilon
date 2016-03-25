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

ExpressionLayout * Power::createLayout() {
  return new ExponentLayout(m_operands[0]->createLayout(), m_operands[1]->createLayout());
}
