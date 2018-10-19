#ifndef POINCARE_EMPTY_EXPRESSION_H
#define POINCARE_EMPTY_EXPRESSION_H

#include <poincare/expression.h>

namespace Poincare {

// An empty expression awaits completion by the user.

class EmptyExpressionNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(EmptyExpressionNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "EmptyExpression";
  }
#endif

  // Properties
  Type type() const override { return Type::EmptyExpression; }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class EmptyExpression final : public Expression {
public:
  EmptyExpression();
  EmptyExpression(const EmptyExpressionNode * n) : Expression(n) {}
};

}

#endif
