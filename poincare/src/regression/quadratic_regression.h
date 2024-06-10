#ifndef POINCARE_REGRESSION_QUADRATIC_REGRESSION_H
#define POINCARE_REGRESSION_QUADRATIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class QuadraticRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 3; }

  Poincare::Layout templateLayout() const override;

  double evaluate(double* modelCoefficients, double x) const override;

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  double partialDerivate(double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
