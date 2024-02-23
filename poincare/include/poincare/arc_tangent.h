#ifndef POINCARE_ARC_TANGENT_H
#define POINCARE_ARC_TANGENT_H

#include <poincare/approximation_helper.h>
#include <poincare/arc_cotangent.h>
#include <poincare/expression.h>
#include <poincare/trigonometry.h>

namespace Poincare {

class ArcTangentNode final : public ExpressionNode {
  friend class ArcCotangentNode;

 public:
  constexpr static AliasesList k_functionName = AliasesLists::k_atanAliases;

  // TreeNode
  size_t size() const override { return sizeof(ArcTangentNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcTangent";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context* context) const override {
    return childAtIndex(0)->isPositive(context);
  }
  TrinaryBoolean isNull(Context* context) const override {
    return childAtIndex(0)->isNull(context);
  }
  Type type() const override { return Type::ArcTangent; }

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

class ArcTangent final : public ExpressionOneChild<ArcTangent, ArcTangentNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
