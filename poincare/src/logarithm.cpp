#include <poincare/logarithm.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

Logarithm::Logarithm() :
  Function("log")
{
}

Expression::Type Logarithm::type() const {
  return Type::Logarithm;
}

Expression * Logarithm::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Logarithm * l = new Logarithm();
  l->setArgument(newOperands, numberOfOperands, cloneOperands);
  return l;
}

float Logarithm::approximate(Context& context) const {
  return log10f(m_args[0]->approximate(context));
}
