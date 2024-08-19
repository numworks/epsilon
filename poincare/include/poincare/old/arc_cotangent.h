#ifndef POINCARE_ARC_COTANGENT_H
#define POINCARE_ARC_COTANGENT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ArcCotangentNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "arccot";

  // PoolObject
  size_t size() const override { return sizeof(ArcCotangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcCotangent";
  }
#endif

  // Properties
  Type otype() const override { return Type::ArcCotangent; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplification
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

class ArcCotangent final
    : public ExpressionOneChild<ArcCotangent, ArcCotangentNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
