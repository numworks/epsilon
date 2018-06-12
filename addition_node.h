#ifndef ADDITION_NODE_H
#define ADDITION_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class AdditionNode : public ExpressionNode {
public:
  AdditionNode() : ExpressionNode() {
    printf("Create Addition\n");
  }
#if TREE_LOGGING
  const char * description() const override {
    return "Addition";
  }
#endif
  float approximate() override {
    float result = 0.0f;
    for (int i=0; i<numberOfOperands(); i++) {
      result += operand(i)->approximate();
    }
    return result;
  }
/*
  Expression simplify() override {
    // Scan operands, merge constants
    Addition a = wrapped(this);
    if (operand(0)->type() == Integer && operand(1)->type() == Integer) {
    }
  }
  */
};

#include "end_node.h"

class Addition : public ExpressionReference<AdditionNode> {
public:
  Addition(Expression e1, Expression e2) :
    ExpressionReference<AdditionNode>()
  {
    ExpressionReference<EndNode> end;
    ExpressionNode::Pool()->log();
    addOperand(end);
    ExpressionNode::Pool()->log();
  }
};


//typedef ExpressionReference<AdditionNode> Addition;

#endif
