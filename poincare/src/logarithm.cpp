#include <poincare/logarithm.h>
#include <poincare/float.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

Logarithm::Logarithm() :
  Function("log")
{
}

Expression::Type Logarithm::type() const {
  return Expression::Type::Logarithm;
}

Expression * Logarithm::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Logarithm * c = new Logarithm();
  c->setArgument(*newOperands, cloneOperands);
  return c;
}

float Logarithm::approximate(Context& context) const {
  return log10f(m_arg->approximate(context));
}

Expression * Logarithm::createEvaluation(Context& context) const {
  return new Float(log10f(m_arg->approximate(context)));
}
