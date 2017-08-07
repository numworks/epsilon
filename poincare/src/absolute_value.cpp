#include <poincare/absolute_value.h>
#include <poincare/complex.h>
#include "layout/absolute_value_layout.h"

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

AbsoluteValue::AbsoluteValue() :
  Function("abs")
{
}

Expression::Type AbsoluteValue::type() const {
  return Type::AbsoluteValue;
}

Expression * AbsoluteValue::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  AbsoluteValue * a = new AbsoluteValue();
  a->setArgument(newOperands, numberOfOperands, cloneOperands);
  return a;
}

Complex AbsoluteValue::computeComplex(const Complex c, AngleUnit angleUnit) const {
  return Complex::Float(c.r());
}

ExpressionLayout * AbsoluteValue::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new AbsoluteValueLayout(m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

}
