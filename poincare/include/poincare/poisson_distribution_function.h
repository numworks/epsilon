#ifndef POINCARE_POISSON_DISTRIBUTION_FUNCTION_H
#define POINCARE_POISSON_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>

namespace Poincare {

// PoissonDistributionFunctions are PoissonCDF and PoissonPDF

class PoissonDistributionFunctionNode : public ExpressionNode {
private:
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class PoissonDistributionFunction : public Expression {
public:
  PoissonDistributionFunction(const PoissonDistributionFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(Context * context, bool * stopReduction = nullptr);
};

}

#endif
