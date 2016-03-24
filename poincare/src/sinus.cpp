#include <poincare/sinus.h>
#include "layout/horizontal_layout.h"

Expression * Sinus::clone() {
  return new Sinus(m_arg, true);
}

Expression::Type Sinus::type() {
  return Expression::Type::Sinus;
}

float Sinus::approximate(Context& context) {
  // FIXME: use sinus obviously.
  return m_arg->approximate(context);
}
