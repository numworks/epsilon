#ifndef FLOAT_NODE_H
#define FLOAT_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class FloatNode : public ExpressionNode {
public:
  FloatNode() : ExpressionNode() {}
  size_t size() const override {
    return sizeof(FloatNode);
  }
  float approximate() override {
    return m_value;
  }
#if TREE_LOGGING
  const char * description() const override {
    if (m_value > 1) {
      return "BigFloat";
    } else {
      return "SmallFloat";
    }
  }
#endif
  void setFloat(float f) { m_value = f; }
private:
  float m_value;
};

class Float : public ExpressionReference<FloatNode> {
public:
  Float(float f) : ExpressionReference<FloatNode>() {
    this->node()->setFloat(f);
  }
};

//typedef ExpressionReference<FloatNode> Float;

#endif
