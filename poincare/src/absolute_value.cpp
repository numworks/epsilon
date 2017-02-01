#include <poincare/absolute_value.h>
#include "layout/absolute_value_layout.h"

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
  AbsoluteValue * a = new AbsoluteValue();
  a->setArgument(newOperands, numberOfOperands, cloneOperands);
  return a;
}

float AbsoluteValue::approximate(Context& context, AngleUnit angleUnit) const {
  return fabsf(m_args[0]->approximate(context, angleUnit));
}

ExpressionLayout * AbsoluteValue::createLayout(DisplayMode displayMode) const {
  return new AbsoluteValueLayout(m_args[0]->createLayout(displayMode));
}