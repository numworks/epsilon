#include <poincare/undefined.h>
#include <poincare/complex.h>
extern "C" {
#include <math.h>
}
#include "layout/string_layout.h"

namespace Poincare {

Expression::Type Undefined::type() const {
  return Type::Undefined;
}

Expression * Undefined::clone() const {
  return new Undefined();
}

Evaluation<float> * Undefined::privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const {
  return new Complex<float>(Complex<float>::Float(NAN));
}

Evaluation<double> * Undefined::privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const {
  return new Complex<double>(Complex<double>::Float(NAN));
}

ExpressionLayout * Undefined::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  char buffer[16];
  int numberOfChars = Complex<float>::convertFloatToText(NAN, buffer, 16, 0, floatDisplayMode);
  return new StringLayout(buffer, numberOfChars);
}

}

