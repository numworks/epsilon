#include <poincare/empty_expression.h>
#include <poincare/allocation_failure_expression_node.h>
#include <poincare/complex.h>
#include <poincare/empty_layout_node.h>
#include <poincare/serialization_helper.h>
#include <ion/charset.h>

namespace Poincare {

EmptyExpressionNode * EmptyExpressionNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<EmptyExpressionNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

int EmptyExpressionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Char(buffer, bufferSize, Ion::Charset::Empty);
}

LayoutRef EmptyExpressionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return EmptyLayoutRef();
}

template<typename T> Evaluation<T> EmptyExpressionNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return Complex<T>::Undefined();
}

}
