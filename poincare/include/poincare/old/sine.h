#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include "approximation_helper.h"
#include "old_expression.h"
#include "trigonometry.h"

namespace Poincare {

class SineNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "sin";

  // PoolObject
  size_t size() const override { return sizeof(SineNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Sine"; }
#endif

  // Properties
  Type otype() const override { return Type::Sine; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplication
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext) override;
};

class Sine final : public ExpressionOneChild<Sine, SineNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
