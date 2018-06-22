#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "tree_node.h"

class ExpressionNode : public TreeNode {
public:
  virtual float approximate() = 0;
  ExpressionNode * child(int i) { return static_cast<ExpressionNode *>(childTreeAtIndex(i)); }
};

#endif
