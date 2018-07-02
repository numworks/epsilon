#include "expression_reference.h"
#include "allocation_failed_expression_node.h"

template<>
TreeNode * ExpressionRef::FailedAllocationStaticNode() {
  static AllocationFailedExpressionNode FailureNode;
  if (FailureNode.identifier() >= -1) {
    int newIdentifier = TreePool::sharedPool()->registerStaticNode(&FailureNode);
    FailureNode.rename(newIdentifier);
  }
  return &FailureNode;
}
