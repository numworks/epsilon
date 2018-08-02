#include <poincare/infinity.h>
#include <poincare/complex.h>
#include <poincare/layout_engine.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

LayoutRef InfinityNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[5];
  int numberOfChars = writeTextInBuffer(buffer, 5, floatDisplayMode, numberOfSignificantDigits);
  return LayoutEngine::createStringLayout(buffer, numberOfChars);
}

int InfinityNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return PrintFloat::convertFloatToText<float>(m_negative ? -INFINITY : INFINITY, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
}

template<typename T> EvaluationReference<T> InfinityNode::templatedApproximate() const {
  return ComplexReference<T>(INFINITY);
}

}

