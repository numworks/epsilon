#ifndef POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H
#define POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class ProportionalRegression : public Regression {
 public:
  using Regression::Regression;

  Type type() const override { return Type::Proportional; }

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 private:
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const Coefficients& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
