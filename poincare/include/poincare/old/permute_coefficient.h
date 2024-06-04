#ifndef POINCARE_PERMUTE_COEFFICIENT_H
#define POINCARE_PERMUTE_COEFFICIENT_H

#include "evaluation.h"
#include "old_expression.h"

namespace Poincare {

class PermuteCoefficientNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "permute";

  // PoolObject
  size_t size() const override { return sizeof(PermuteCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "PermuteCoefficient";
  }
#endif

  // ExpressionNode

  // Properties
  Type otype() const override { return Type::PermuteCoefficient; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class PermuteCoefficient final
    : public ExpressionTwoChildren<PermuteCoefficient, PermuteCoefficientNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);

  constexpr static int k_maxNValue = 100;
};

}  // namespace Poincare

#endif
