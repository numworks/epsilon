#include <poincare/fraction.h>
#include <string.h>
#include "layout/fraction_layout.h"

Fraction::Fraction(Expression * numerator, Expression * denominator) :
  m_numerator(numerator),
  m_denominator(denominator) {
}

Fraction::~Fraction() {
  delete m_denominator;
  delete m_numerator;
}

ExpressionLayout * Fraction::createLayout(ExpressionLayout * parent) {
  return new FractionLayout(parent, m_numerator, m_denominator);
}

float Fraction::approximate(Context& context) {
  // TODO: handle division by zero
  return m_numerator->approximate(context)/m_denominator->approximate(context);
}

Expression::Type Fraction::type() {
  return Expression::Type::Fraction;
}
