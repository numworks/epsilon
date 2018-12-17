#ifndef POINCARE_FACTOR_H
#define POINCARE_FACTOR_H

#include <poincare/expression.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>
#include <cmath>

namespace Poincare {

class FactorNode /*final*/ : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(FactorNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Factor";
  }
#endif
  Type type() const override { return Type::Factor; }
private:
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Serialization */
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
    return childAtIndex(0)->approximate(T(), context, angleUnit);
  }
};

class Factor final : public Expression {
public:
  Factor(const FactorNode * n) : Expression(n) {}
  static Factor Builder(Expression child) { return Factor(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("factor", 1, &UntypedBuilder);

  Expression shallowBeautify(Context & context, Preferences::AngleUnit angleUnit);
  Multiplication createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context & context, Preferences::AngleUnit angleUnit) const;
private:
  explicit Factor(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<FactorNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
};

}
#endif
