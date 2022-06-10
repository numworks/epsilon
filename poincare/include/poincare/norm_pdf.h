#ifndef POINCARE_NORM_PDF_H
#define POINCARE_NORM_PDF_H

#include <poincare/approximation_helper.h>
#include <poincare/normal_distribution_function.h>

namespace Poincare {

class NormPDFNode final : public NormalDistributionFunctionNode  {
public:
  static constexpr char k_functionName[] = "normpdf";

  // TreeNode
  size_t size() const override { return sizeof(NormPDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NormPDF";
  }
#endif

  // Properties
  Type type() const override { return Type::NormPDF; }
  Sign sign(Context * context) const override { return Sign::Positive; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};

class NormPDF final : public HandleThreeChildren<NormPDF, NormPDFNode, NormalDistributionFunction> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
};

}

#endif
