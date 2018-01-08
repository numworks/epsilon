#include <poincare/undefined.h>
#include <poincare/layout_engine.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

Expression::Type Undefined::type() const {
  return Type::Undefined;
}

Expression * Undefined::clone() const {
  return new Undefined();
}

template<typename T> Complex<T> * Undefined::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  return new Complex<T>(Complex<T>::Float(NAN));
}

ExpressionLayout * Undefined::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  char buffer[16];
  int numberOfChars = Complex<float>::convertFloatToText(NAN, buffer, 16, 1, floatDisplayMode);
  return LayoutEngine::createStringLayout(buffer, numberOfChars);
}

int Undefined::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  return strlcpy(buffer, "undef", bufferSize);
}

}

