#include <poincare/cosine.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

Cosine::Cosine() :
  Function("cos")
{
}

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Cosine * c = new Cosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

float Cosine::approximate(Context& context) const {
  return cosf(m_args[0]->approximate(context));
}
