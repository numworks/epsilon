#ifndef POINCARE_EXCEPTION_EXPRESSION_NODE_H
#define POINCARE_EXCEPTION_EXPRESSION_NODE_H

#include <poincare/exception_node.h>
#include <poincare/expression_node.h>
#include <poincare/expression.h>
#include <poincare/complex.h>
#include <poincare/char_layout_node.h>
#include <stdio.h>

namespace Poincare {

/* All exceptions should be caught so ExceptionExpressionNode's methods are
 * asserted false. */
class ExceptionExpressionNode : public ExceptionNode<ExpressionNode> {
public:
  // ExpressionNode
  ExpressionNode::Sign sign() const override { assert(false); return ExpressionNode::Sign::Unknown; }
  ExpressionNode::Type type() const override { assert(false); return ExpressionNode::Type::Exception; } // TODO assert(false) ?
  Evaluation<float> approximate(ExpressionNode::SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return Complex<float>::Undefined(); }
  Evaluation<double> approximate(ExpressionNode::DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { assert(false); return Complex<double>::Undefined(); }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override {
    assert(false); 
    if (bufferSize == 0) {
      return -1;
    }
    return PrintFloat::convertFloatToText<float>(NAN, buffer, bufferSize, numberOfSignificantDigits, floatDisplayMode);
  }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return LayoutRef(CharLayoutNode::FailedAllocationStaticNode()); }
  // FIXME: Use EmptyLayoutNode here above, once EmptyLayout has been cleaned up

  // TreeNode
  TreeNode * failedAllocationStaticNode() override { /*TODO*/ assert(false); return nullptr; }
  size_t size() const override { return sizeof(ExceptionExpressionNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ExceptionExpression";
  }
#endif
};

class ExceptionExpression : public Expression {
public:
  ExceptionExpression() : Expression(ExceptionExpressionStaticNode()) {}
private:
  static ExceptionExpressionNode * ExceptionExpressionStaticNode();
};

}

#endif
