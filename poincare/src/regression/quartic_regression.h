#ifndef POINCARE_REGRESSION_QUARTIC_REGRESSION_H
#define POINCARE_REGRESSION_QUARTIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class QuarticRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 5; }

  const char* formula() const override { return "y=a·x^4+b·x^3+c·x^2+d·x+e"; }
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
