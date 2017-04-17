#include <poincare/factorial.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Factorial::Factorial(Expression * argument, bool clone) :
  Function("fact")
{
  setArgument(&argument, 1, clone);
}

Expression::Type Factorial::type() const {
  return Type::Factorial;
}

Expression * Factorial::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Factorial * f = new Factorial(newOperands[0], cloneOperands);
  return f;
}

float Factorial::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float n = m_args[0]->approximate(context, angleUnit);
  if (isnan(n) || n != (int)n || n < 0.0f) {
    return NAN;
  }
  float result = 1.0f;
  for (int i = 1; i <= (int)n; i++) {
    result *= (float)i;
    if (isinf(result)) {
      return result;
    }
  }
  return roundf(result);
}

ExpressionLayout * Factorial::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = m_args[0]->createLayout(floatDisplayMode, complexFormat);
  childrenLayouts[1] = new StringLayout("!", 1);
  return new HorizontalLayout(childrenLayouts, 2);
}

}
