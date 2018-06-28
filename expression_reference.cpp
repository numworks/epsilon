#include "expression_reference.h"
#include "allocation_failed_expression_node.h"

template<>
TreeNode * ExpressionRef::failedAllocationNode() {
  static AllocationFailedExpressionRef FailureRef;
  return FailureRef.node();
}

template<>
ExpressionReference<ExpressionNode> ExpressionRef::failedAllocationRef() {
  return ExpressionReference<ExpressionNode>(failedAllocationNode());
}
