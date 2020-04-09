#ifndef POINCARE_BINOM_CDF_H
#define POINCARE_BINOM_CDF_H

#include <poincare/approximation_helper.h>
#include <poincare/binomial_distribution_function.h>

namespace Poincare {

class BinomCDFNode final : public BinomialDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(BinomCDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BinomCDF";
  }
#endif

  // Properties
  Type type() const override { return Type::BinomCDF; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  Expression setSign(Sign s, ReductionContext reductionContext) override;

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class BinomCDF final : public BinomialDistributionFunction {
public:
  BinomCDF(const BinomCDFNode * n) : BinomialDistributionFunction(n) {}
  static BinomCDF Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BinomCDF, BinomCDFNode>({child0, child1, child2}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("binomcdf", 3, &UntypedBuilderThreeChildren<BinomCDF>);
};

}

#endif
