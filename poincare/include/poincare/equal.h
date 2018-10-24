#ifndef POINCARE_EQUAL_H
#define POINCARE_EQUAL_H

#include <poincare/expression.h>

namespace Poincare {

class EqualNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(EqualNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Equal";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Equal; }
  int polynomialDegree(Context & context, const char * symbolName) const override { return -1; }
private:
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evalutation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class Equal final : public Expression {
public:
  Equal(const EqualNode * n) : Expression(n) {}
  Equal(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<EqualNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }

  // For the equation A = B, create the reduced expression A-B
  Expression standardEquation(Context & context, Preferences::AngleUnit angleUnit) const;
  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
};

}

#endif
