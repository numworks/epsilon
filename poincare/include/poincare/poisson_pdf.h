#ifndef POINCARE_POISSON_PDF_H
#define POINCARE_POISSON_PDF_H

#include <poincare/approximation_helper.h>
#include <poincare/poisson_distribution_function.h>

namespace Poincare {

class PoissonPDFNode final : public PoissonDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(PoissonPDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "PoissonPDF";
  }
#endif

  // Properties
  Type type() const override { return Type::PoissonPDF; }
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

class PoissonPDF final : public PoissonDistributionFunction {
public:
  PoissonPDF(const PoissonPDFNode * n) : PoissonDistributionFunction(n) {}
  static PoissonPDF Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<PoissonPDF, PoissonPDFNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("poissonpdf", 2, &UntypedBuilderTwoChildren<PoissonPDF>);
};

}

#endif
