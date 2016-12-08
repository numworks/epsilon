#include <poincare/sine.h>
#include <poincare/float.h>

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
  s->setArgument(*newOperands, cloneOperands);
  return s;
}

Expression::Type Sine::type() const {
  return Expression::Type::Sine;
}

float Sine::approximate(Context& context) const {
  return sinf(m_arg->approximate(context));
}

Expression * Sine::evaluate(Context& context) const {
  return new Float(sinf(m_arg->approximate(context)));
}