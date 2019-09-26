#include <poincare/infinity.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

Expression InfinityNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive || s == ExpressionNode::Sign::Negative);
  return Infinity(this).setSign(s);
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
  return PrintFloat::ConvertFloatToText<float>(m_negative ? -INFINITY : INFINITY, buffer, bufferSize, PrintFloat::k_maxFloatGlyphLength, numberOfSignificantDigits, floatDisplayMode).CharLength;
}

template<typename T> Evaluation<T> InfinityNode::templatedApproximate() const {
  return Complex<T>::Builder(m_negative ? -INFINITY : INFINITY);
}

Infinity Infinity::Builder(bool negative) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(InfinityNode));
  InfinityNode * node = new (bufferNode) InfinityNode(negative);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Infinity &>(h);
}

Expression Infinity::setSign(ExpressionNode::Sign s) {
  assert(s == ExpressionNode::Sign::Positive || s == ExpressionNode::Sign::Negative);
  Expression result = Infinity::Builder(s == ExpressionNode::Sign::Negative);
  replaceWithInPlace(result);
  return result;
}

template Evaluation<float> InfinityNode::templatedApproximate<float>() const;
template Evaluation<double> InfinityNode::templatedApproximate() const;
}
