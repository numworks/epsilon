#ifndef POINCARE_ARC_COSINE_H
#define POINCARE_ARC_COSINE_H

#include "approximation_helper.h"
#include "arc_secant.h"
#include "old_expression.h"
#include "trigonometry.h"

namespace Poincare {

class ArcCosineNode final : public ExpressionNode {
  friend class ArcSecantNode;

 public:
  constexpr static AliasesList k_functionName = AliasesLists::k_acosAliases;

  // PoolObject
  size_t size() const override { return sizeof(ArcCosineNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcCosine";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context) == OMG::Troolean::Unknown
               ? OMG::Troolean::Unknown
               : OMG::Troolean::True;
  }
  Type otype() const override { return Type::ArcCosine; }

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

  // Derivation
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext) override;
};

class ArcCosine final : public ExpressionOneChild<ArcCosine, ArcCosineNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
