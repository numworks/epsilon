#include "expression_reference.h"
#include "allocation_failed_expression_node.h"

template<>
TreeNode * ExpressionRef::staticFailedAllocationStaticNode() {
  static AllocationFailedExpressionRef FailureRef;
  return FailureRef.node();
}

template<>
ExpressionReference<ExpressionNode> ExpressionRef::staticFailedAllocationStaticRef() {
  return ExpressionReference<ExpressionNode>(staticFailedAllocationStaticNode());
}
