#include <poincare/cosine.h>
#include "layout/horizontal_layout.h"

Expression * Cosine::clone() {
  return new Cosine(m_arg, true);
}

Expression::Type Cosine::type() {
  return Expression::Type::Cosine;
}

float Cosine::approximate(Context& context) {
  // FIXME: use cosine obviously.
  return m_arg->approximate(context);
}
