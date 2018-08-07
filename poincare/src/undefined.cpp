#include <poincare/undefined.h>
#include <poincare/complex.h>
#include <poincare/layout_engine.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

int UndefinedNode::polynomialDegree(char symbolName) const {
  return -1;
}

LayoutRef UndefinedNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[6];
  int numberOfChars = PrintFloat::convertFloatToText<float>(NAN, buffer, 6, numberOfSignificantDigits, floatDisplayMode);
  return LayoutEngine::createStringLayout(buffer, numberOfChars);
}

int UndefinedNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return PrintFloat::convertFloatToText<float>(NAN, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
}

template<typename T> Evaluation<T> UndefinedNode::templatedApproximate() const {
  return Complex<T>::Undefined();
}

}

