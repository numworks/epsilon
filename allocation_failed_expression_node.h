#ifndef ALLOCATION_FAILED_EXPRESSION_NODE_H
#define ALLOCATION_FAILED_EXPRESSION_NODE_H

#include "expression_node.h"
#include "expression_reference.h"
#include <stdio.h>

class AllocationFailedExpressionNode : public ExpressionNode {
public:
  // ExpressionNode
  float approximate() override { return -1; } // Should return nan

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedExpressionNode); }
  const char * description() const override { return "Allocation Failed";  }
  Type type() const override { return Type::AllocationFailure; }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};

class AllocationFailedExpressionRef : public ExpressionReference<AllocationFailedExpressionNode> {
public:
  using ExpressionReference<AllocationFailedExpressionNode>::ExpressionReference;
};

#endif
