#ifndef POINCARE_PERMUTE_COEFFICIENT_H
#define POINCARE_PERMUTE_COEFFICIENT_H

#include <poincare/evaluation.h>
#include <poincare/expression.h>

namespace Poincare {

class PermuteCoefficientNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "permute";

  // TreeNode
  size_t size() const override { return sizeof(PermuteCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "PermuteCoefficient";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::PermuteCoefficient; }
  TrinaryBoolean isPositive(Context* context) const override {
    return TrinaryBoolean::True;
  }

 private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class PermuteCoefficient final
    : public ExpressionTwoChildren<PermuteCoefficient, PermuteCoefficientNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  // Expression
  Expression shallowReduce(ReductionContext reductionContext);

  constexpr static int k_maxNValue = 100;
};

}  // namespace Poincare

#endif
