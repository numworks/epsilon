#ifndef POINCARE_NORM_CDF_RANGE_H
#define POINCARE_NORM_CDF_RANGE_H

#include <poincare/approximation_helper.h>
#include <poincare/normal_distribution_function.h>

namespace Poincare {

class NormCDFRangeNode final : public NormalDistributionFunctionNode  {
public:
  constexpr static AliasesList k_functionName = "normcdfrange";

  // TreeNode
  size_t size() const override { return sizeof(NormCDFRangeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NormCDFRange";
  }
#endif

  // Properties
  Type type() const override { return Type::NormCDFRange; }
  TrinaryBoolean isPositive(Context * context) const override { return TrinaryBoolean::True; }
  int muIndex() const override { return 2; }
  int varIndex() const override { return 3; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class NormCDFRange final : public ExpressionFourChildren<NormCDFRange, NormCDFRangeNode, NormalDistributionFunction> {
public:
  using ExpressionBuilder::ExpressionBuilder;
};

}

#endif
