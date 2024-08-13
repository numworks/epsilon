#ifndef POINCARE_REGRESSION_CUBIC_REGRESSION_H
#define POINCARE_REGRESSION_CUBIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class CubicRegression : public Regression {
 public:
  using Regression::Regression;

  Type type() const override { return Type::Cubic; }
  double evaluate(const double* modelCoefficients, double x) const override;

 private:
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
