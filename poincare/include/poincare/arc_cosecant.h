#ifndef POINCARE_ARC_COSCANT_H
#define POINCARE_ARC_COSCANT_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class ArcCosecantNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "arccsc";

  // PoolObject
  size_t size() const override { return sizeof(ArcCosecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ArcCosecant";
  }
#endif

  // Properties
  Type otype() const override { return Type::ArcCosecant; }

  template <typename T>
  static std::complex<T> computeOnComplex(
      const std::complex<T> c, Preferences::ComplexFormat complexFormat,
      Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplication
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
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

class ArcCosecant final
    : public ExpressionOneChild<ArcCosecant, ArcCosecantNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
