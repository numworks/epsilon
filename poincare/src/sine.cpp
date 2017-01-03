#include <poincare/sine.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

Sine::Sine() :
  Function("sin")
{
}

Expression * Sine::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  Sine * s = new Sine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

float Sine::approximate(Context& context) const {
  return sinf(m_args[0]->approximate(context));
}
