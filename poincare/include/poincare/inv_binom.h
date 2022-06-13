#ifndef POINCARE_INV_BINOM_H
#define POINCARE_INV_BINOM_H

#include <poincare/approximation_helper.h>
#include <poincare/binomial_distribution_function.h>

namespace Poincare {

class InvBinomNode final : public BinomialDistributionFunctionNode  {
public:
  static constexpr char k_functionName[] = "invbinom";

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
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class InvBinom final : public ExpressionThreeChildren<InvBinom, InvBinomNode, BinomialDistributionFunction> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
