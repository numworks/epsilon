#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "tree_node.h"

class ExpressionNode : public TreeNode {
public:
  static TreeNode * FailedAllocationStaticNode();
  static int AllocationFailureNodeIdentifier() {
    return FailedAllocationStaticNode()->identifier();
  }
  int allocationFailureNodeIdentifier() override {
    return AllocationFailureNodeIdentifier();
  }

  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  virtual float approximate() = 0;
  ExpressionNode * child(int i) { return static_cast<ExpressionNode *>(childTreeAtIndex(i)); }
};

#endif
