#ifndef POINCARE_NORM_CDF_H
#define POINCARE_NORM_CDF_H

#include <poincare/approximation_helper.h>
#include <poincare/normal_distribution_function.h>

namespace Poincare {

class NormCDFNode final : public NormalDistributionFunctionNode  {
public:
  static constexpr char k_functionName[] = "normcdf";

  // TreeNode
  size_t size() const override { return sizeof(NormCDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NormCDF";
  }
#endif

  // Properties
  Type type() const override { return Type::NormCDF; }
  Sign sign(Context * context) const override { return Sign::Positive; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class NormCDF final : public ExpressionThreeChildren<NormCDF, NormCDFNode, NormalDistributionFunction> {
public:
  using ExpressionBuilder::ExpressionBuilder;
};

}

#endif
