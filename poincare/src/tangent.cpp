#include <poincare/tangent.h>

extern "C" {
#include <assert.h>
}

Expression * Tangent::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  return new Tangent(*newOperands, cloneOperands);
}

Expression::Type Tangent::type() {
  return Expression::Type::Tangent;
}

float Tangent::approximate(Context& context) {
  // FIXME: use tangent obviously.
  return m_arg->approximate(context);
}
