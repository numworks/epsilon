#ifndef POINCARE_ALLOCATION_FAILURE_EXPRESSION_NODE_H
#define POINCARE_ALLOCATION_FAILURE_EXPRESSION_NODE_H

#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/complex.h>
#include <poincare/allocation_failure_layout_node.h>
#include <poincare/char_layout_node.h>
#include <stdio.h>

namespace Poincare {

template <typename T>
class AllocationFailureExpressionNode : public T {
public:
  AllocationFailureExpressionNode() : T() {}
  AllocationFailureExpressionNode(T node) : T(node) {}

  // ExpressionNode
  ExpressionNode::Sign sign() const override { return ExpressionNode::Sign::Unknown; }
  ExpressionNode::Type type() const override { return ExpressionNode::Type::AllocationFailure; }
  Evaluation<float> approximate(ExpressionNode::SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined(); }
  Evaluation<double> approximate(ExpressionNode::DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined(); }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    if (bufferSize == 0) {
      return -1;
    }
    return PrintFloat::convertFloatToText<float>(NAN, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
  }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { return LayoutRef(CharLayoutNode::FailedAllocationStaticNode()); }
  // FIXME: Use EmptyLayoutNode here above, once EmptyLayout has been cleaned up

  // TreeNode
  size_t size() const override { return sizeof(AllocationFailureExpressionNode<T>); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailureExpression";  }
#endif
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};


}

#endif
