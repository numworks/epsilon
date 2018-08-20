#include <poincare/infinity.h>
#include <poincare/allocation_failure_expression_node.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>

extern "C" {
#include <math.h>
#include <string.h>
}

namespace Poincare {

InfinityNode * InfinityNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<InfinityNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression InfinityNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  return Infinity(s == Sign::Negative);
}

LayoutRef InfinityNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
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
  return Complex<T>(INFINITY);
}

}

