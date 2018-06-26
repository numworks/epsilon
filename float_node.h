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
  int numberOfChildren() const override { return 0; }
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

class FloatRef : public ExpressionReference<FloatNode> {
public:
  FloatRef(float f) : ExpressionReference<FloatNode>() {
    this->node()->setFloat(f);
  }
};

#endif
