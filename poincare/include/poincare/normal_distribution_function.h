#ifndef POINCARE_NORMAL_DISTRIBUTION_FUNCTION_H
#define POINCARE_NORMAL_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>

namespace Poincare {

// NormalDistributionFunctions are NormCDF, NormCDF2, InvNorm and NormPDF

class NormalDistributionFunctionNode : public ExpressionNode {
private:
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class NormalDistributionFunction : public Expression {
public:
  NormalDistributionFunction(const NormalDistributionFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(Context * context, bool * stopReduction = nullptr);
};

}

#endif
