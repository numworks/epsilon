#ifndef POINCARE_BINOMIAL_DISTRIBUTION_FUNCTION_H
#define POINCARE_BINOMIAL_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>

namespace Poincare {

// BinomialDistributionFunctions are BinomCDF, InvBinom and BinomPDF

class BinomialDistributionFunctionNode : public ExpressionNode {
private:
  // Simplication
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class BinomialDistributionFunction : public Expression {
public:
  BinomialDistributionFunction(const BinomialDistributionFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext, bool * stopReduction = nullptr);
};

}

#endif
