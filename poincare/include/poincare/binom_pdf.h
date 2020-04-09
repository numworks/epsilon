#ifndef POINCARE_BINOM_PDF_H
#define POINCARE_BINOM_PDF_H

#include <poincare/approximation_helper.h>
#include <poincare/binomial_distribution_function.h>

namespace Poincare {

class BinomPDFNode final : public BinomialDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(BinomPDFNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BinomPDF";
  }
#endif

  // Properties
  Type type() const override { return Type::BinomPDF; }
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

class BinomPDF final : public BinomialDistributionFunction {
public:
  BinomPDF(const BinomPDFNode * n) : BinomialDistributionFunction(n) {}
  static BinomPDF Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BinomPDF, BinomPDFNode>({child0, child1, child2}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("binompdf", 3, &UntypedBuilderThreeChildren<BinomPDF>);
};

}

#endif
