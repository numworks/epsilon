#ifndef POINCARE_FACTOR_H
#define POINCARE_FACTOR_H

#include <cmath>

#include "multiplication.h"
#include "old_expression.h"
#include "rational.h"

namespace Poincare {

class FactorNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "factor";

  // PoolObject
  size_t size() const override { return sizeof(FactorNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Factor"; }
#endif
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  OMG::Troolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }
  Type otype() const override { return Type::Factor; }

 private:
  /* Layout */
  /* Serialization */
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  /* Simplification */
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class Factor final : public ExpressionOneChild<Factor, FactorNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  Multiplication createMultiplicationOfIntegerPrimeDecomposition(
      Integer i) const;

  // OExpression
  OExpression shallowBeautify(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare
#endif
