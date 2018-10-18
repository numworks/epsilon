#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression.h>

namespace Poincare {

class ParenthesisNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ParenthesisNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Parenthesis";
  }
#endif

  // Properties
  Type type() const override { return Type::Parenthesis; }
  int polynomialDegree(Context & context, const char * symbolName) const override;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
private:
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Parenthesis final : public Expression {
public:
  Parenthesis(const ParenthesisNode * n) : Expression(n) {}
  Parenthesis(Expression exp) : Expression(TreePool::sharedPool()->createTreeNode<ParenthesisNode>()) {
    replaceChildAtIndexInPlace(0, exp);
  }
  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
};

}

#endif
