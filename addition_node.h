#ifndef ADDITION_NODE_H
#define ADDITION_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class AdditionNode : public ExpressionNode {
public:
#if TREE_LOGGING
  const char * description() const override {
    return "Addition";
  }
#endif

  size_t size() const override {
    return sizeof(AdditionNode);
  }
  float approximate() override {
    float result = 0.0f;
    for (int i=0; i<numberOfChildren(); i++) {
      result += child(i)->approximate();
    }
    return result;
  }

  int numberOfChildren() const override {
    return 2;
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

class Addition : public ExpressionReference<AdditionNode> {
public:
  Addition(Expression e1, Expression e2) :
    ExpressionReference<AdditionNode>()
  {
    addOperand(e2);
    addOperand(e1);
  }
};


//typedef ExpressionReference<AdditionNode> Addition;

#endif
