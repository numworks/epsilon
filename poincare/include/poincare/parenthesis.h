#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class ParenthesisNode : public ExpressionNode {
public:
  static ParenthesisNode * FailedAllocationStaticNode();

  // TreeNode
  size_t size() const override { return sizeof(ParenthesisNode); }
#if TREE_LOG
  const char * description() const override { return "Parenthesis";  }
#endif
  int numberOfChildren() const override { return 1; }

  // Properties
  Type type() const override { return Type::Parenthesis; }
  int polynomialDegree(char symbolName) const override;

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
private:
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Parenthesis : public Expression {
public:
  Parenthesis(const ParenthesisNode * n) : Expression(n) {}
  Parenthesis(Expression exp) :
    Expression(TreePool::sharedPool()->createTreeNode<ParenthesisNode>())
  {
    replaceChildAtIndexInPlace(0, exp);
  }
  // Expression
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
