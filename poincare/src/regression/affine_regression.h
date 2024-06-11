#ifndef POINCARE_REGRESSION_AFFINE_REGRESSION_H
#define POINCARE_REGRESSION_AFFINE_REGRESSION_H

#include <apps/global_preferences.h>

#include "regression.h"

namespace Poincare::Regression {

/* This is a pure virtual class that factorises all regression models that
 * compute an affine function (linear model and median-median model) */
class AffineRegression : public Regression {
 public:
  using Regression::Regression;
  int numberOfCoefficients() const override { return 2; }

  const char* formula() const override {
    // TODO_PCJ: mx+b
    return "y=a·x+b";
  }
  double evaluate(const double* modelCoefficients, double x) const override;
  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 protected:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  virtual int slopeCoefficientIndex() const { return 0; }
  virtual int yInterceptCoefficientIndex() const { return 1; }

 private:
  void privateFit(const Series* series, double* modelCoefficients,
                  Poincare::Context* context) const override = 0;
};

}  // namespace Poincare::Regression

#endif
