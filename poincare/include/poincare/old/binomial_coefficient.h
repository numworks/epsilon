#ifndef POINCARE_BINOMIAL_COEFFICIENT_H
#define POINCARE_BINOMIAL_COEFFICIENT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class BinomialCoefficientNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "binomial";

  // PoolObject
  size_t size() const override { return sizeof(BinomialCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "BinomialCoefficient";
  }
#endif

  // Properties
  Type otype() const override { return Type::BinomialCoefficient; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };
};

class BinomialCoefficient final
    : public ExpressionTwoChildren<BinomialCoefficient,
                                   BinomialCoefficientNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);

 private:
  constexpr static int k_maxNValue = 300;
};

}  // namespace Poincare

#endif
