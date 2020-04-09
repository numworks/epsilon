#ifndef POINCARE_NORM_CDF2_H
#define POINCARE_NORM_CDF2_H

#include <poincare/approximation_helper.h>
#include <poincare/normal_distribution_function.h>

namespace Poincare {

class NormCDF2Node final : public NormalDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(NormCDF2Node); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "NormCDF2";
  }
#endif

  // Properties
  Type type() const override { return Type::NormCDF2; }
  Sign sign(Context * context) const override { return Sign::Positive; }
  Expression setSign(Sign s, ReductionContext reductionContext) override;
  int muIndex() const override { return 2; }
  int varIndex() const override { return 3; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class NormCDF2 final : public NormalDistributionFunction {
public:
  NormCDF2(const NormCDF2Node * n) : NormalDistributionFunction(n) {}
  static NormCDF2 Builder(Expression child0, Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<NormCDF2, NormCDF2Node>({child0, child1, child2, child3}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("normcdf2", 4, &UntypedBuilderFourChildren<NormCDF2>);
};

}

#endif
