extern "C" {
#include <assert.h>
#include <string.h>
}

#include <poincare/fraction.h>
#include "layout/fraction_layout.h"

Expression * Fraction::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  return new Fraction(newOperands, cloneOperands);
}

ExpressionLayout * Fraction::createLayout() {
  return new FractionLayout(m_operands[0]->createLayout(), m_operands[1]->createLayout());
}

float Fraction::approximate(Context& context) {
  // TODO: handle division by zero
  return m_operands[0]->approximate(context)/m_operands[1]->approximate(context);
}

Expression::Type Fraction::type() {
  return Expression::Type::Fraction;
}
