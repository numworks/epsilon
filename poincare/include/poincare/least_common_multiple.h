#ifndef POINCARE_LEAST_COMMON_MULTIPLE_H
#define POINCARE_LEAST_COMMON_MULTIPLE_H

#include <poincare/n_ary_expression.h>
#include <limits.h>

namespace Poincare {

class LeastCommonMultipleNode final : public NAryExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(LeastCommonMultipleNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "LeastCommonMultiple";
  }
#endif

  // ExpressionNode
  Sign sign(Context * context) const override { return Sign::Positive; }
  Type type() const override { return Type::LeastCommonMultiple; }

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

class LeastCommonMultiple final : public NAryExpression {
public:
  LeastCommonMultiple(const LeastCommonMultipleNode * n) : NAryExpression(n) {}
  static LeastCommonMultiple Builder(const Tuple & children = {}) { return TreeHandle::NAryBuilder<LeastCommonMultiple, LeastCommonMultipleNode>(convert(children)); }
  constexpr static Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("lcm", 2, INT_MAX, &UntypedBuilderMultipleChildren<LeastCommonMultiple>);

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  Expression shallowBeautify(Context * context);
};

}

#endif
