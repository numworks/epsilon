#ifndef POINCARE_DIVISION_QUOTIENT_H
#define POINCARE_DIVISION_QUOTIENT_H

#include <poincare/expression.h>
#include <poincare/integer.h>

namespace Poincare {

class DivisionQuotientNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(DivisionQuotientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "DivisionQuotient";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::DivisionQuotient; }

  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class DivisionQuotient final : public Expression {
public:
  DivisionQuotient(const DivisionQuotientNode * n) : Expression(n) {}
  static DivisionQuotient Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<DivisionQuotient, DivisionQuotientNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("quo", 2, &UntypedBuilderTwoChildren<DivisionQuotient>);

  // Expression
  Expression shallowReduce(Context * context);
  static Expression Reduce(const Integer & a, const Integer & b);
};

}

#endif
