#include "expression_node.h"
#include "allocation_failed_expression_node.h"
#include "expression_reference.h"

TreeNode * ExpressionNode::FailedAllocationStaticNode() {
  return ExpressionRef::FailedAllocationStaticNode();
}
