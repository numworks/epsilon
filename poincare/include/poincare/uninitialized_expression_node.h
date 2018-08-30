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
  static int UninitializedExpressionStaticNodeIdentifier() { return -2; }

  // ExpressionNode
  void setChildrenInPlace(Expression other) override { assert(false); return ExceptionExpressionNode<ExpressionNode>::setChildrenInPlace(other); }
  ExpressionNode::Sign sign() const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::sign(); }
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::setSign(s, context, angleUnit); }

  ExpressionNode::Type type() const override { assert(false); return ExpressionNode::Type::Uninitialized; }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    assert(false);
    return ExceptionExpressionNode<ExpressionNode>::serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::createLayout(floatDisplayMode, numberOfSignificantDigits); }

  // TreeNode
  bool isUninitialized() const override { return true; }
  /* There is only one static node, that should never be inserted in the pool,
   * so no need for an allocation failure. */
  TreeNode * failedAllocationStaticNode() override { assert(false); return nullptr; }
  size_t size() const override { return sizeof(UninitializedExpressionNode); }
  Expression denominator(Context & context, Preferences::AngleUnit angleUnit) const override { assert(false); return ExceptionExpressionNode<ExpressionNode>::denominator(context, angleUnit); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "UninitializedExpression";
  }
#endif
};

}

#endif
