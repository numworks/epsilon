#ifndef POINCARE_PERMUTE_COEFFICIENT_H
#define POINCARE_PERMUTE_COEFFICIENT_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class PermuteCoefficientNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(PermuteCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "PermuteCoefficient";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override{ return Type::PermuteCoefficient; }
  Sign sign(Context * context) const override { return Sign::Positive; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class PermuteCoefficient final : public Expression {
public:
  PermuteCoefficient(const PermuteCoefficientNode * n) : Expression(n) {}
  static PermuteCoefficient Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<PermuteCoefficient, PermuteCoefficientNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("permute", 2, Initializer<PermuteCoefficientNode>, sizeof(PermuteCoefficientNode));

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

  constexpr static int k_maxNValue = 100;
};

}

#endif
