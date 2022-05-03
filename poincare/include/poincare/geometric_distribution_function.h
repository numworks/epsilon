#ifndef POINCARE_GEOMETRIC_DISTRIBUTION_FUNCTION_H
#define POINCARE_GEOMETRIC_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>

namespace Poincare {

// GeometricDistributionFunctions are GeomCDF, GeomCDFRange, InvGeom and GeomPDF

class GeometricDistributionFunctionNode : public ExpressionNode {
private:
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class GeometricDistributionFunction : public Expression {
public:
  GeometricDistributionFunction(const GeometricDistributionFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(Context * context, bool * stopReduction = nullptr);
};

}

#endif
