#include <poincare/parenthesis.h>
#include <poincare/allocation_failure_expression_node.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

ParenthesisNode * ParenthesisNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<ParenthesisNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

int ParenthesisNode::polynomialDegree(char symbolName) const {
  return childAtIndex(0)->polynomialDegree(symbolName);
}

LayoutRef ParenthesisNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

int ParenthesisNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "");
}

Expression ParenthesisNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Parenthesis(this).shallowReduce(context, angleUnit);
}

template<typename T>
Evaluation<T> ParenthesisNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return childAtIndex(0)->approximate(T(), context, angleUnit);
}

Expression Parenthesis::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::shallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  return childAtIndex(0);
}

}
