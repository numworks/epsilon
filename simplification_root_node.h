#ifndef SIMPLIFICATION_ROOT_NODE_H
#define SIMPLIFICATION_ROOT_NODE_H

#include "expression_reference.h"
#include "expression_node.h"

class SimplificationRootNode : public ExpressionNode {
public:
  SimplificationRootNode() : ExpressionNode() {}
  size_t size() const override { return sizeof(SimplificationRootNode); }
  Type type() const override { return Type::SimplificationRoot; }
  int numberOfChildren() const override { return 1; }
  float approximate() override { assert(false); return -1; }
  const char * description() const override { return "Simplification root"; }
};

class SimplificationRootRef : public ExpressionReference<SimplificationRootNode> {
public:
  using ExpressionReference<SimplificationRootNode>::ExpressionReference;
  SimplificationRootRef(ExpressionRef c) :
    SimplificationRootRef()
  {
    addChild(c);
  }
};

#endif
