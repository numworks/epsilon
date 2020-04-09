#ifndef POINCARE_INV_NORM_H
#define POINCARE_INV_NORM_H

#include <poincare/approximation_helper.h>
#include <poincare/normal_distribution_function.h>

namespace Poincare {

class InvNormNode final : public NormalDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(InvNormNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "InvNorm";
  }
#endif

  // Properties
  Type type() const override { return Type::InvNorm; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class InvNorm final : public NormalDistributionFunction {
public:
  InvNorm(const InvNormNode * n) : NormalDistributionFunction(n) {}
  static InvNorm Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<InvNorm, InvNormNode>({child0, child1, child2}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("invnorm", 3, &UntypedBuilderThreeChildren<InvNorm>);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
