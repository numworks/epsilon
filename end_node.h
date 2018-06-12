#ifndef END_NODE_H
#define END_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class EndNode : public ExpressionNode {
public:
  EndNode() : ExpressionNode() {
    printf("Create end node\n");
  }
  float approximate() override {
    assert(false);
    return 0.0f;
  }
#if TREE_LOGGING
  const char * description() const override {
    return "End";
  }
#endif
};

//typedef ExpressionReference<EndNode> End;

#endif
