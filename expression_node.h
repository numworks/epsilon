#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "tree_node.h"

class ExpressionNode : public TreeNode {
public:
  ExpressionNode() : TreeNode(TreePool::sharedPool()->generateIdentifier()) {
    TreePool::sharedPool()->registerNode(this);
  }

  virtual float approximate() = 0;
  int numberOfOperands() { return numberOfChildren(); }
  ExpressionNode * operand(int i) { return static_cast<ExpressionNode *>(childAtIndex(i)); }
};

#endif
