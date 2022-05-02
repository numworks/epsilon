#ifndef POINCARE_POISSON_CDF_H
#define POINCARE_POISSON_CDF_H

#include <poincare/approximation_helper.h>
#include <poincare/poisson_distribution_function.h>

namespace Poincare {

class PoissonCDFNode final : public PoissonDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(PoissonCDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "PoissonCDF";
  }
#endif

  // Properties
  Type type() const override { return Type::PoissonCDF; }
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

class PoissonCDF final : public PoissonDistributionFunction {
public:
  PoissonCDF(const PoissonCDFNode * n) : PoissonDistributionFunction(n) {}
  static PoissonCDF Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<PoissonCDF, PoissonCDFNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("poissoncdf", 2, &UntypedBuilderTwoChildren<PoissonCDF>);
};

}

#endif
