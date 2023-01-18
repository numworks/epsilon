#ifndef POINCARE_BINOMIAL_COEFFICIENT_H
#define POINCARE_BINOMIAL_COEFFICIENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class BinomialCoefficientNode final : public ExpressionNode {
public:
  constexpr static AliasesList k_functionName = "binomial";

  // TreeNode
  size_t size() const override { return sizeof(BinomialCoefficientNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "BinomialCoefficient";
  }
#endif

  // Properties
  Type type() const override{ return Type::BinomialCoefficient; }
  template<typename T> static T compute(T k, T n);
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class BinomialCoefficient final : public ExpressionTwoChildren<BinomialCoefficient, BinomialCoefficientNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;

  // Expression
  Expression shallowReduce(ReductionContext reductionContext);
private:
  constexpr static int k_maxNValue = 300;
};

}

#endif
