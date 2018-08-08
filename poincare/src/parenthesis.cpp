#include <poincare/parenthesis.h>
#include <poincare/allocation_failure_expression_node.h>
#include <assert.h>

namespace Poincare {

ParenthesisNode * ParenthesisNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<ParenthesisNode> failure;
  return &failure;
}

int ParenthesisNode::polynomialDegree(char symbolName) const {
  return childAtIndex(0)->polynomialDegree(symbolName);
}

LayoutRef ParenthesisNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::createParenthesedLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

Expression ParenthesisNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  Expression e = ExpressionNode::shallowReduce(context, angleUnit);
  if (e.node() != this) {
    return e;
  }
  return Expression(childAtIndex(0));
}

template<typename T>
Evaluation<T> ParenthesisNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return childAtIndex(0)->approximate(T(), context, angleUnit);
}

Expression Parenthesis::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::shallowReduce(context, angleUnit);
  if (e != *this) {
    return e;
  }
  return childAtIndex(0);
}

}
