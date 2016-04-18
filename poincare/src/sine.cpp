#include <poincare/sine.h>

extern "C" {
#include <assert.h>
}

Expression * Sine::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  return new Sine(*newOperands, cloneOperands);
}

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

float Sine::approximate(Context& context) const {
  // FIXME: use sine obviously.
  return m_arg->approximate(context);
}
