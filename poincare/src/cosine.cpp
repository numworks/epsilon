extern "C" {
#include <assert.h>
}

#include <poincare/cosine.h>
#include "layout/horizontal_layout.h"

Expression::Type Cosine::type() {
  return Expression::Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  return new Cosine(*newOperands, cloneOperands);
}

float Cosine::approximate(Context& context) {
  // FIXME: use cosine obviously.
  return m_arg->approximate(context);
}
