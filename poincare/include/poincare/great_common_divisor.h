#ifndef POINCARE_GREAT_COMMON_DIVISOR_H
#define POINCARE_GREAT_COMMON_DIVISOR_H

#include <poincare/n_ary_expression.h>

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
  Sign sign(Context * context) const override { return Sign::Positive; }
  Type type() const override { return Type::GreatCommonDivisor; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowBeautify(ReductionContext * reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class GreatCommonDivisor final : public NAryExpression {
public:
  GreatCommonDivisor(const GreatCommonDivisorNode * n) : NAryExpression(n) {}
  static GreatCommonDivisor Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<GreatCommonDivisor, GreatCommonDivisorNode>(convert(children)); }
  // Using a -2 as numberOfChildren to allow 2 or more children when parsing
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("gcd", -2, &UntypedBuilderMultipleChildren<GreatCommonDivisor>);

  // Expression
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
  Expression shallowBeautify(Context * context);
};

}

#endif
