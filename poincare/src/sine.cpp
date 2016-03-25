#include <poincare/sine.h>
#include "layout/horizontal_layout.h"

Expression * Sine::clone() {
  return new Sine(m_arg, true);
}

Expression::Type Sine::type() {
  return Expression::Type::Sine;
}

float Sine::approximate(Context& context) {
  // FIXME: use sine obviously.
  return m_arg->approximate(context);
}
