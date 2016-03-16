#include <poincare/power.h>
#include "layout/exponent_layout.h"

Power::Power(Expression * base, Expression * exponent) :
  m_base(base),
  m_exponent(exponent) {
}

Power::~Power() {
  delete m_exponent;
  delete m_base;
}

float Power::approximate(Context& context) {
  // TODO: do this for real
  return 1;
}

ExpressionLayout * Power::createLayout(ExpressionLayout * parent) {
  return new ExponentLayout(parent, m_base, m_exponent);
}
