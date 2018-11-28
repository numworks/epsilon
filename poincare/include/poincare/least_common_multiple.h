#ifndef POINCARE_LEAST_COMMON_MULTIPLE_H
#define POINCARE_LEAST_COMMON_MULTIPLE_H

#include <poincare/expression.h>

namespace Poincare {

class LeastCommonMultipleNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(LeastCommonMultipleNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "LeastCommonMultiple";
  }
#endif
  // ExpressionNode
  Type type() const override { return Type::LeastCommonMultiple; }
private:
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class LeastCommonMultiple final : public Expression {
public:
  LeastCommonMultiple(const LeastCommonMultipleNode * n) : Expression(n) {}
  static LeastCommonMultiple Builder(Expression child0, Expression child1) { return LeastCommonMultiple(child0, child1); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0), children.childAtIndex(1)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("lcm", 2, &UntypedBuilder);

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
private:
  LeastCommonMultiple(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<LeastCommonMultipleNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
};

}

#endif
