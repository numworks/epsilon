#ifndef POINCARE_GREAT_COMMON_DIVISOR_H
#define POINCARE_GREAT_COMMON_DIVISOR_H

#include <poincare/n_ary_expression.h>
#include <limits.h>

namespace Poincare {

class GreatCommonDivisorNode final : public NAryExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(GreatCommonDivisorNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "GreatCommonDivisor";
  }
#endif

  // ExpressionNode
  TrinaryBoolean isPositive(Context * context) const override { return TrinaryBoolean::True; }
  Type type() const override { return Type::GreatCommonDivisor; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class GreatCommonDivisor final : public NAryExpression {
public:
  GreatCommonDivisor(const GreatCommonDivisorNode * n) : NAryExpression(n) {}
  static GreatCommonDivisor Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<GreatCommonDivisor, GreatCommonDivisorNode>(convert(children)); }
  constexpr static Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("gcd", 2, INT_MAX, &UntypedBuilderMultipleChildren<GreatCommonDivisor>);

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(Context * context);
};

}

#endif
