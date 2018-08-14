#ifndef POINCARE_UNINITIALIZED_EXPRESSION_NODE_H
#define POINCARE_UNINITIALIZED_EXPRESSION_NODE_H

#include <poincare/exception_expression_node.h>
#include <stdio.h>

namespace Poincare {

/* All UninitializedExpressions should be caught so its node methods are
 * asserted false. */

class UninitializedExpressionNode : public ExceptionExpressionNode<ExpressionNode> {
public:
  static UninitializedExpressionNode * UninitializedExpressionStaticNode();

  // ExpressionNode
  ExpressionNode::Sign sign() const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::sign(); }
  ExpressionNode::Type type() const override { assert(false); return ExpressionNode::Type::Uninitialized; } // TODO assert(false) ?
  Evaluation<float> approximate(ExpressionNode::SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::approximate(p, context, angleUnit); }
  Evaluation<double> approximate(ExpressionNode::DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::approximate(p, context, angleUnit); }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    assert(false);
    return ExceptionExpressionNode<ExpressionNode>::serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); ExceptionExpressionNode<ExpressionNode>::createLayout(floatDisplayMode, numberOfSignificantDigits); }

  // TreeNode
  bool isUninitialized() const override { return true; }
  TreeNode * failedAllocationStaticNode() override { /*TODO*/ assert(false); return nullptr; /* Or call parent method ?*/ }
  size_t size() const override { return sizeof(UninitializedExpressionNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedExpression";
  }
#endif
};

}

#endif
