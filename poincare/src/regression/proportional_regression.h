#ifndef POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H
#define POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class ProportionalRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 1; }

  const char* formula() const override { return "y=a·x"; }
  double evaluate(const double* modelCoefficients, double x) const override;
  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
