#include <poincare/binomial_coefficient.h>
#include <poincare/evaluation.h>
#include <poincare/complex.h>
#include "layout/parenthesis_layout.h"
#include "layout/grid_layout.h"

extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>

namespace Poincare {

BinomialCoefficient::BinomialCoefficient() :
  Function("binomial", 2)
{
}

Expression::Type BinomialCoefficient::type() const {
  return Type::BinomialCoefficient;
}

Expression * BinomialCoefficient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  BinomialCoefficient * bc = new BinomialCoefficient();
  bc->setArgument(newOperands, numberOfOperands, cloneOperands);
  return bc;
}

template<typename T>
Evaluation<T> * BinomialCoefficient::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * nInput = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * kInput = m_args[1]->evaluate<T>(context, angleUnit);
  T n = nInput->toScalar();
  T k = kInput->toScalar();
  delete nInput;
  delete kInput;
  if (std::isnan(n) || std::isnan(k) || n != (int)n || k != (int)k || k > n || k < 0 || n < 0) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T result = 1;
  for (int i = 0; i < (int)k; i++) {
    result *= (n-(T)i)/(k-(T)i);
  }
  return new Complex<T>(Complex<T>::Float(std::round(result)));
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

