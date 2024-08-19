#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class TangentNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "tan";

  // PoolObject
  size_t size() const override { return sizeof(TangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Tangent"; }
#endif

  // Properties
  Type otype() const override { return Type::Tangent; }

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

class Tangent final : public ExpressionOneChild<Tangent, TangentNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  OExpression shallowReduce(ReductionContext reductionContext);

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
