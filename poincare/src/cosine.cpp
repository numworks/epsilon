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

float Cosine::approximate(Context& context, AngleUnit angleUnit) const {
  if (angleUnit == AngleUnit::Degree) {
    return cosf(m_args[0]->approximate(context, angleUnit)*M_PI/180.0f);
  }
  return cosf(m_args[0]->approximate(context, angleUnit));
}

//TODO: implement evaluate to handle cos complex