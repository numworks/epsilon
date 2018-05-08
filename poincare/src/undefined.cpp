#include <poincare/undefined.h>
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

int Undefined::polynomialDegree(char symbolName) const {
  return -1;
}

template<typename T> Complex<T> * Undefined::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Float(NAN));
}

ExpressionLayout * Undefined::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  char buffer[16];
  int numberOfChars = PrintFloat::convertFloatToText<float>(NAN, buffer, 16, 1, floatDisplayMode);
  return new StringLayout(buffer, numberOfChars);
}

int Undefined::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return strlcpy(buffer, "undef", bufferSize);
}

}

