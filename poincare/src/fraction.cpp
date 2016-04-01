#include <poincare/fraction.h>
#include <string.h>
#include "layout/fraction_layout.h"

Expression * Fraction::clone() {
  return new Fraction(m_operands, true);
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

#ifdef DEBUG
int Fraction::getPrintableVersion(char* txt) {
  txt[0] = '/';
  return 1;
}
#endif
