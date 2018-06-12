#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "tree_node.h"

class TreePool;

class ExpressionNode : public TreeNode {
public:
  ExpressionNode(int identifier) : TreeNode(identifier) {}

  static TreePool * Pool() {
    static TreePool pool;
    return &pool;
  }

  virtual float approximate() = 0;
  int numberOfOperands() { return numberOfChildren(); }
  ExpressionNode * operand(int i) { return static_cast<ExpressionNode *>(childAtIndex(i)); }
};

#endif
