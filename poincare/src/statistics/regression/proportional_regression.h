#pragma once

#include <poincare/statistics/regression.h>

namespace Poincare::Internal {

class ProportionalRegression : public Regression {
 public:
  Type type() const override { return Type::Proportional; }

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, const Poincare::Context& context) const override;

 private:
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const Coefficients& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Internal
