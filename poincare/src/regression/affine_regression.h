#ifndef POINCARE_REGRESSION_AFFINE_REGRESSION_H
#define POINCARE_REGRESSION_AFFINE_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

/* This is a pure virtual class that factorises all regression models that
 * compute an affine function (linear model and median-median model) */
class AffineRegression : public Regression {
 public:
  using Regression::Regression;

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  virtual int slopeCoefficientIndex() const { return 0; }
  virtual int yInterceptCoefficientIndex() const { return 1; }

 private:
  double privateEvaluate(const CoefficientsType& modelCoefficients,
                         double x) const override;
};

}  // namespace Poincare::Regression

#endif
