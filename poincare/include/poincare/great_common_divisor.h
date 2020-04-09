#ifndef POINCARE_GREAT_COMMON_DIVISOR_H
#define POINCARE_GREAT_COMMON_DIVISOR_H

#include <poincare/expression.h>

namespace Poincare {

class GreatCommonDivisorNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(GreatCommonDivisorNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "GreatCommonDivisor";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::GreatCommonDivisor; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class GreatCommonDivisor final : public Expression {
public:
  GreatCommonDivisor(const GreatCommonDivisorNode * n) : Expression(n) {}
  static GreatCommonDivisor Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<GreatCommonDivisor, GreatCommonDivisorNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("gcd", 2, &UntypedBuilderTwoChildren<GreatCommonDivisor>);

  // Expression
  Expression shallowReduce(Context * context);
};

}

#endif
