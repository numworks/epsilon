#ifndef ALLOCATION_FAILED_EXPRESSION_NODE_H
#define ALLOCATION_FAILED_EXPRESSION_NODE_H

#include "expression_node.h"

class AllocationFailedExpressionNode : public ExpressionNode {
public:
  // ExpressionNode
  float approximate() override { return -1; } // Should return nan

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedExpressionNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
};

class AllocationFailedExpressionRef : public ExpressionReference<AllocationFailedExpressionNode> {
public:
  AllocationFailedExpressionRef() : ExpressionReference<AllocationFailedExpressionNode>() {}
};

#endif
