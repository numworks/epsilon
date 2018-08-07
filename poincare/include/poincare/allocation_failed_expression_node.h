#ifndef POINCARE_ALLOCATION_FAILED_EXPRESSION_NODE_H
#define POINCARE_ALLOCATION_FAILED_EXPRESSION_NODE_H

#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/complex.h>
#include <poincare/allocation_failed_layout_node.h>
#include <stdio.h>

namespace Poincare {

class AllocationFailedExpressionNode : public ExpressionNode {
public:
  static AllocationFailedExpressionNode * FailedAllocationStaticNode() {
    return static_cast<AllocationFailedExpressionNode *>(ExpressionNode::FailedAllocationStaticNode());
  }

  // ExpressionNode
  Type type() const override { return Type::AllocationFailure; }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined(); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined(); }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    int descriptionLength = strlen(description()) + 1;
    return strlcpy(buffer, description(), bufferSize < descriptionLength ? bufferSize : descriptionLength);
  }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return AllocationFailedLayoutRef(); }

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailedExpressionNode); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailedExpression";  }
#endif
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};

class AllocationFailedExpressionRef : public Expression {
public:
  AllocationFailedExpressionRef() : Expression(TreePool::sharedPool()->createTreeNode<AllocationFailedExpressionNode>()) {}
};

}

#endif
