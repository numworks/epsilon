#ifndef POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H
#define POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class ProportionalRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 1; }

  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) const override;

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  double partialDerivate(double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
