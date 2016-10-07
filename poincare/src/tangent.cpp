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
  t->setArgument(*newOperands, cloneOperands);
  return t;
}

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

float Tangent::approximate(Context& context) const {
  // return tanf(m_arg->approximate(context));
  return m_arg->approximate(context);
}
