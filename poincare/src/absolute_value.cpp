#include <poincare/absolute_value.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

AbsoluteValue::AbsoluteValue() :
  Function("abs")
{
}

Expression::Type AbsoluteValue::type() const {
  return Type::AbsoluteValue;
}

Expression * AbsoluteValue::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  AbsoluteValue * c = new AbsoluteValue();
  c->setArgument(*newOperands, cloneOperands);
  return c;
}

float AbsoluteValue::approximate(Context& context) const {
  return fabsf(m_arg->approximate(context));
}
