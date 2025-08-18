#ifndef POINCARE_NORM_CDF_RANGE_H
#define POINCARE_NORM_CDF_RANGE_H

#include "approximation_helper.h"
#include "normal_distribution_function.h"

namespace Poincare {

class NormCDFRangeNode final : public NormalDistributionFunctionNode {
 public:
  constexpr static AliasesList k_functionName = "normcdfrange";

  // PoolObject
  size_t size() const override { return sizeof(NormCDFRangeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "NormCDFRange";
  }
#endif

  // Properties
  Type otype() const override { return Type::NormCDFRange; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
  }
  int muIndex() const override { return 2; }
  int varIndex() const override { return 3; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class NormCDFRange final
    : public ExpressionFourChildren<NormCDFRange, NormCDFRangeNode,
                                    NormalDistributionFunction> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
};

}  // namespace Poincare

#endif
