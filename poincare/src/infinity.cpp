#include <poincare/infinity.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

Expression InfinityNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return Infinity(this).setSign(s, context, angleUnit);
}

Layout InfinityNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[5];
  int numberOfChars = serialize(buffer, 5, floatDisplayMode, numberOfSignificantDigits);
  return LayoutHelper::String(buffer, numberOfChars);
}

int InfinityNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  return PrintFloat::convertFloatToText<float>(m_negative ? -INFINITY : INFINITY, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
}

template<typename T> Evaluation<T> InfinityNode::templatedApproximate() const {
  return Complex<T>(m_negative ? -INFINITY : INFINITY);
}

Expression Infinity::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  Expression result = Infinity(s == ExpressionNode::Sign::Negative);
  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> InfinityNode::templatedApproximate<float>() const;
template Evaluation<double> InfinityNode::templatedApproximate() const;
}
