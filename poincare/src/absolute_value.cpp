#include <poincare/absolute_value.h>
#include <poincare/complex.h>
#include "layout/absolute_value_layout.h"

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type AbsoluteValue::type() const {
  return Type::AbsoluteValue;
}

Expression * AbsoluteValue::clone() const {
  AbsoluteValue * a = new AbsoluteValue(m_operands, true);
  return a;
}

Expression * AbsoluteValue::immediateSimplify(Context& context, AngleUnit angleUnit) {
  if (operand(0)->sign() > 0) {
    return replaceWith(editableOperand(0), true);
  }
  if (operand(0)->sign() < 0) {
    Expression * op = editableOperand(0);
    Expression * newOp = op->turnIntoPositive(context, angleUnit);
    return replaceWith(newOp, true);
  }
  return this;
}

template<typename T>
Complex<T> AbsoluteValue::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.r());
}

ExpressionLayout * AbsoluteValue::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new AbsoluteValueLayout(operand(0)->createLayout(floatDisplayMode, complexFormat));
}

}
