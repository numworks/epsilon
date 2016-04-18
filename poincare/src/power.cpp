extern "C" {
#include <assert.h>
#include <math.h>
}

#include <poincare/power.h>
#include "layout/exponent_layout.h"

float Power::approximate(Context& context) const {
  return powf(m_operands[0]->approximate(context), m_operands[1]->approximate(context));
}

Expression::Type Power::type() const {
  return Expression::Type::Power;
}

Expression * Power::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  return new Power(newOperands, cloneOperands);
}

ExpressionLayout * Power::createLayout() const {
  return new ExponentLayout(m_operands[0]->createLayout(), m_operands[1]->createLayout());
}
