#include <poincare/absolute_value.h>
#include <poincare/complex.h>
#include <poincare/simplification_engine.h>
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

Expression * AbsoluteValue::setSign(Sign s, Context & context, AngleUnit angleUnit) {
  assert(s == Sign::Positive);
  return this;
}

ExpressionLayout * AbsoluteValue::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new AbsoluteValueLayout(operand(0)->createLayout(floatDisplayMode, complexFormat));
}

Expression * AbsoluteValue::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  if (op->sign() == Sign::Positive) {
    return replaceWith(op, true);
  }
  if (op->sign() == Sign::Negative) {
    Expression * newOp = op->setSign(Sign::Positive, context, angleUnit);
    return replaceWith(newOp, true);
  }
  return this;
}

template<typename T>
Complex<T> AbsoluteValue::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.r());
}

}
