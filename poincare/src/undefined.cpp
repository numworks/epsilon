#include <poincare/undefined.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

int UndefinedNode::polynomialDegree(char symbolName) const {
  return -1;
}

Expression UndefinedNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return Undefined();
}

Layout UndefinedNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[6];
  int numberOfChars = PrintFloat::convertFloatToText<float>(NAN, buffer, 6, numberOfSignificantDigits, floatDisplayMode);
  return LayoutHelper::String(buffer, numberOfChars);
}

int UndefinedNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return PrintFloat::convertFloatToText<float>(NAN, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
}

template<typename T> Evaluation<T> UndefinedNode::templatedApproximate() const {
  return Complex<T>::Undefined();
}

template Evaluation<float> UndefinedNode::templatedApproximate() const;
template Evaluation<double> UndefinedNode::templatedApproximate() const;
}

