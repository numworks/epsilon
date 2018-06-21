#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "tree_node.h"
#include "tree_pool.h"

class ExpressionNode : public TreeNode {
public:
  ExpressionNode() : TreeNode(Pool()->generateIdentifier()) {
    Pool()->registerNode(this);
  }

  // TODO: operator new and delte
  // this behavior is the same for every TreeNode
  // Find a way to define it on the TreeNode

  void * operator new(size_t count) {
    return Pool()->alloc(count);
  }

  void operator delete(void * ptr) {
    Pool()->dealloc(ptr);
  }

  virtual float approximate() = 0;
  int numberOfOperands() { return numberOfChildren(); }
  ExpressionNode * operand(int i) { return static_cast<ExpressionNode *>(childAtIndex(i)); }
};

#endif

/* Code I want to write:
 *
 *   ExpressionNode * n = new AdditionNode();
 *   delete n;
 */
