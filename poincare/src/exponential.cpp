#include <poincare/exponential.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

Exponential::Exponential() :
  Function("exp")
{
}

Expression::Type Exponential::type() const {
  return Type::Exponential;
}

Expression * Exponential::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Exponential * e = new Exponential();
  e->setArgument(newOperands, numberOfOperands, cloneOperands);
  return e;
}

float Exponential::approximate(Context& context) const {
  return expf(m_args[0]->approximate(context));
}
