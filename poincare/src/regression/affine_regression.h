#ifndef POINCARE_REGRESSION_AFFINE_REGRESSION_H
#define POINCARE_REGRESSION_AFFINE_REGRESSION_H

#include <poincare/regression/regression.h>

namespace Poincare::Regression {

/* This is a pure virtual class that factorises all regression models that
 * compute an affine function (linear model and median-median model) */
class AffineRegression : public Regression {
 public:
  using Regression::Regression;

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 protected:
  virtual int slopeCoefficientIndex() const { return 0; }
  virtual int yInterceptCoefficientIndex() const { return 1; }

  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;

 private:
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;

  Coefficients privateFit(const Series* series,
                          Poincare::Context* context) const override = 0;
};

}  // namespace Poincare::Regression

#endif
