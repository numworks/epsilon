#ifndef POINCARE_REGRESSION_CUBIC_REGRESSION_H
#define POINCARE_REGRESSION_CUBIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class CubicRegression : public Regression {
 public:
  using Regression::Regression;

  Type type() const override { return Type::Cubic; }

 private:
  double privateEvaluate(const CoefficientsType& modelCoefficients,
                         double x) const override;
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const CoefficientsType& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Poincare::Regression

#endif
