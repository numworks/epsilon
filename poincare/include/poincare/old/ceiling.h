#ifndef POINCARE_CEILING_H
#define POINCARE_CEILING_H

#include "approximation_helper.h"
#include "old_expression.h"

namespace Poincare {

class CeilingNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "ceil";

  // PoolObject
  size_t size() const override { return sizeof(CeilingNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Ceiling"; }
#endif

  // Properties
  Type otype() const override { return Type::Ceiling; }

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

class Ceiling final : public ExpressionOneChild<Ceiling, CeilingNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
