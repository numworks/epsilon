#include <poincare/tangent.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

Tangent::Tangent() :
  Function("tan")
{
}

Expression * Tangent::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  assert(numberOfOperands == 1);
  Tangent * t = new Tangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

float Tangent::approximate(Context& context, AngleUnit angleUnit) const {
  if (angleUnit == AngleUnit::Degree) {
    return tanf(m_args[0]->approximate(context, angleUnit)*M_PI/180.0f);
  }
  return tanf(m_args[0]->approximate(context, angleUnit));
}
