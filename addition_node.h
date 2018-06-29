#ifndef ADDITION_NODE_H
#define ADDITION_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class AdditionNode : public ExpressionNode {
public:
  const char * description() const override { return "Addition"; }
  size_t size() const override { return sizeof(AdditionNode); }
  Type type() const override { return Type::Addition; }

  float approximate() override {
    float result = 0.0f;
    for (int i=0; i<numberOfChildren(); i++) {
      float approximateI = child(i)->approximate();
      if (approximateI == -1) {
        return -1;
      }
      result += approximateI;
    }
    return result;
  }

  bool shallowReduce() override {
    if (ExpressionNode::shallowReduce()) {
      return true;
    }
    /* Step 1: Addition is associative, so let's start by merging children which
     * also are additions themselves. */
    int i = 0;
    int initialNumberOfChildren = numberOfChildren();
    while (i < initialNumberOfChildren) {
      ExpressionNode * currentChild = child(i);
      if (currentChild->type() == Type::Addition) {
        TreeRef(this).mergeChildren(TreeRef(currentChild));
        // Is it ok to modify memory while executing ?
        continue;
      }
      i++;
    }

    return false;
  }

  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren() override { m_numberOfChildren++; }
  void decrementNumberOfChildren() override {
    assert(m_numberOfChildren > 0);
    m_numberOfChildren--;
  }
  void eraseNumberOfChildren() override {
    m_numberOfChildren = 0;
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
