#ifndef POINCARE_REGRESSION_QUADRATIC_REGRESSION_H
#define POINCARE_REGRESSION_QUADRATIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class QuadraticRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 3; }

  const char* formula() const override { return "y=a·x^2+b·x+c"; }
  Poincare::Layout templateLayout() const override;

  double evaluate(const double* modelCoefficients, double x) const override;

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
