#ifndef POINCARE_HYPERBOLIC_SINE_H
#define POINCARE_HYPERBOLIC_SINE_H

#include "approximation_helper.h"
#include "hyperbolic_trigonometric_function.h"

namespace Poincare {

class HyperbolicSineNode final : public HyperbolicTrigonometricFunctionNode {
 public:
  constexpr static AliasesList k_functionName = "sinh";

  // PoolObject
  size_t size() const override { return sizeof(HyperbolicSineNode); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "HyperbolicSine";
  }
#endif

  // Properties
  Type otype() const override { return Type::HyperbolicSine; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
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

class HyperbolicSine final
    : public ExpressionOneChild<HyperbolicSine, HyperbolicSineNode,
                                HyperbolicTrigonometricFunction> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);
  OExpression unaryFunctionDifferential(
      const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
