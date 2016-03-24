#include <poincare/sinus.h>
#include "layout/horizontal_layout.h"

Expression::Type Sinus::type() {
  return Expression::Type::Sinus;
}

float Sinus::approximate(Context& context) {
  // FIXME: use sinus obviously.
  return m_arg->approximate(context);
}
