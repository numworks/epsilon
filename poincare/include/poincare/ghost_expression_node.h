#ifndef POINCARE_GHOST_EXPRESSION_NODE_H
#define POINCARE_GHOST_EXPRESSION_NODE_H

#include <poincare/expression_node.h>
#include <poincare/expression_reference.h>
#include <poincare/complex.h>
#include <poincare/ghost_layout_ref.h>
#include <stdio.h>

namespace Poincare {

class GhostExpressionNode : public ExpressionNode {
public:
  // ExpressionNode
  Type type() const override { return Type::Ghost; }
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return ComplexReference<float>::Undefined(); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return ComplexReference<double>::Undefined(); }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return GhostLayoutRef(); }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    int descriptionLength = strlen(description()) + 1;
    return strlcpy(buffer, description(), bufferSize < descriptionLength ? bufferSize : descriptionLength);
  }

  // TreeNode
  size_t size() const override { return sizeof(GhostExpressionNode); }
#if TREE_LOG
  const char * description() const override { return "GhostExpression";  }
#endif
  int numberOfChildren() const override { return 0; }
};

class GhostExpressionRef : public ExpressionReference {
public:
  GhostExpressionRef() : ExpressionReference(TreePool::sharedPool()->createTreeNode<GhostExpressionNode>(), true) {}
  GhostExpressionRef(TreeNode * n) : ExpressionReference(n) {}
};

}

#endif
