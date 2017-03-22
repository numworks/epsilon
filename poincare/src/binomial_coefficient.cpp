#include <poincare/binomial_coefficient.h>
#include <poincare/complex.h>
#include "layout/parenthesis_layout.h"
#include "layout/grid_layout.h"

extern "C" {
#include <stdlib.h>
#include <assert.h>
#include <math.h>
}

namespace Poincare {

BinomialCoefficient::BinomialCoefficient() :
  Function("binomial")
{
}

bool BinomialCoefficient::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 2);
}

Expression::Type BinomialCoefficient::type() const {
  return Type::BinomialCoefficient;
}

Expression * BinomialCoefficient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  BinomialCoefficient * bc = new BinomialCoefficient();
  bc->setArgument(newOperands, numberOfOperands, cloneOperands);
  return bc;
}

float BinomialCoefficient::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float n = m_args[0]->approximate(context, angleUnit);
  float k = m_args[1]->approximate(context, angleUnit);
  if (isnan(n) || isnan(k) || n != (int)n || k != (int)k) {
    return NAN;
  }
  float result = 1.0f;
  for (int i = 0; i < (int)k; i++) {
    result *= (n-(float)i)/(k-(float)i);
  }
  return roundf(result);
}

ExpressionLayout * BinomialCoefficient::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = m_args[0]->createLayout(floatDisplayMode, complexFormat);
  childrenLayouts[1] = m_args[1]->createLayout(floatDisplayMode, complexFormat);
  return new ParenthesisLayout(new GridLayout(childrenLayouts, 2, 1));
}

}

