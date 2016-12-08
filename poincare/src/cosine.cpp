#include <poincare/cosine.h>
#include <poincare/float.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

Cosine::Cosine() :
  Function("cos")
{
}

Expression::Type Cosine::type() const {
  return Expression::Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Cosine * c = new Cosine();
  c->setArgument(*newOperands, cloneOperands);
  return c;
}

float Cosine::approximate(Context& context) const {
  return cosf(m_arg->approximate(context));
}

Expression * Cosine::evaluate(Context& context) const {
  return new Float(cosf(m_arg->approximate(context)));
}
