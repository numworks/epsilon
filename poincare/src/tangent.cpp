#include <poincare/tangent.h>

extern "C" {
#include <assert.h>
}

Expression * Tangent::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  return new Tangent(*newOperands, cloneOperands);
}

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

float Tangent::approximate(Context& context) const {
  // FIXME: use tangent obviously.
  return m_arg->approximate(context);
}
