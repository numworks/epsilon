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
  TrinaryBoolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context) == TrinaryBoolean::Unknown
               ? TrinaryBoolean::Unknown
               : TrinaryBoolean::True;
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

  // Evaluation
  template <typename T>
  static std::complex<T> computeOnComplex(
      const std::complex<T> c, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit);
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<float>(this, approximationContext,
                                                   computeOnComplex<float>);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapOneChild<double>(this, approximationContext,
                                                    computeOnComplex<double>);
  }
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
