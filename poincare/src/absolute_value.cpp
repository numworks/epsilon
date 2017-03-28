#include <poincare/absolute_value.h>
#include <poincare/complex.h>
#include "layout/absolute_value_layout.h"

extern "C" {
#include <assert.h>
#include <math.h>
}

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

float AbsoluteValue::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  Expression * evaluation = m_args[0]->evaluate(context, angleUnit);
  assert(evaluation->type() == Type::Matrix || evaluation->type() == Type::Complex);
  float result = 0.0f;
  if (evaluation->type() == Type::Matrix) {
    result = NAN;
  } else {
    result = ((Complex *)evaluation)->r();
  }
  delete evaluation;
  return result;
}

ExpressionLayout * AbsoluteValue::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new AbsoluteValueLayout(m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

}
