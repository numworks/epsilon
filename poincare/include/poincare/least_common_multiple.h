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
  void logNodeName(std::ostream & stream) const override {
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
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  /* Evaluation */
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class LeastCommonMultiple final : public Expression {
public:
  LeastCommonMultiple(const LeastCommonMultipleNode * n) : Expression(n) {}
  static LeastCommonMultiple Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<LeastCommonMultiple, LeastCommonMultipleNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("lcm", 2, &UntypedBuilderTwoChildren<LeastCommonMultiple>);

  // Expression
  Expression shallowReduce(Context * context);
};

}

#endif
