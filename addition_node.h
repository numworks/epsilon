#ifndef ADDITION_NODE_H
#define ADDITION_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class AdditionNode : public ExpressionNode {
public:
  const char * description() const override {
    return "Addition";
  }

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

  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren() override { m_numberOfChildren++; }
  void decrementNumberOfChildren() override {
    assert(m_numberOfChildren > 0);
    m_numberOfChildren--;
  }
/*
  Expression simplify() override {
    // Scan operands, merge constants
    Addition a = wrapped(this);
    if (operand(0)->type() == Integer && operand(1)->type() == Integer) {
    }
  }
  */
private:
  int m_numberOfChildren;
};

class AdditionRef : public ExpressionReference<AdditionNode> {
public:
  AdditionRef(ExpressionRef e1, ExpressionRef e2) :
    ExpressionReference<AdditionNode>()
  {
    addChild(e2);
    addChild(e1);
  }
};

#endif
