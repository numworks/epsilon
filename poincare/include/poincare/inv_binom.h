#ifndef POINCARE_INV_BINOM_H
#define POINCARE_INV_BINOM_H

#include <poincare/approximation_helper.h>
#include <poincare/binomial_distribution_function.h>

namespace Poincare {

class InvBinomNode final : public BinomialDistributionFunctionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(InvBinomNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "InvBinom";
  }
#endif

  // Properties
  Type type() const override { return Type::InvBinom; }

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

class InvBinom final : public BinomialDistributionFunction {
public:
  InvBinom(const InvBinomNode * n) : BinomialDistributionFunction(n) {}
  static InvBinom Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<InvBinom, InvBinomNode>({child0, child1, child2}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("invbinom", 3, &UntypedBuilderThreeChildren<InvBinom>);
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
