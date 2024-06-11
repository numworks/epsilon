#ifndef POINCARE_REGRESSION_CUBIC_REGRESSION_H
#define POINCARE_REGRESSION_CUBIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class CubicRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 4; }

  const char* formula() const override { return "y=a·x^3+b·x^2+c·x+d"; }
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
