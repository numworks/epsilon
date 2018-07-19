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

  bool shallowReduce() override;
  int numberOfChildren() const override { return m_numberOfChildren; }
  void incrementNumberOfChildren(int increment = 1) override { m_numberOfChildren+= increment; }
  void decrementNumberOfChildren(int decrement = 1) override {
    assert(m_numberOfChildren > 0);
    m_numberOfChildren-= decrement;
  }
  void eraseNumberOfChildren() override {
    m_numberOfChildren = 0;
  }

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
