#ifndef FLOAT_NODE_H
#define FLOAT_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class FloatNode : public ExpressionNode {
public:
  FloatNode(float value = 0) :
    ExpressionNode(),
    m_value(value)
  {}
  size_t size() const override { return sizeof(FloatNode); }
  Type type() const override { return Type::Float; }
  int numberOfChildren() const override { return 0; }
  float approximate() override { return m_value; }
  const char * description() const override {
    return m_value > 1 ? "BigFloat" : "SmallFloat";
  }
  void setFloat(float f) { m_value = f; }
  void init(float f) override { m_value = f; }
private:
  float m_value;
};

class FloatRef : public ExpressionReference<FloatNode> {
public:
  FloatRef(float f) : ExpressionReference<FloatNode>() {
    this->node()->init(f);
  }
};

#endif
