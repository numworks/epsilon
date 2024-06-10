#ifndef POINCARE_REGRESSION_AFFINE_REGRESSION_H
#define POINCARE_REGRESSION_AFFINE_REGRESSION_H

#include <apps/global_preferences.h>

#include "regression.h"

namespace Regression {

/* This is a pure virtual class that factorises all regression models that
 * compute an affine function (linear model and median-median model) */
class AffineRegression : public Regression {
 public:
  using Regression::Regression;
  I18n::Message formulaMessage() const override {
    return UseMxpbForm() ? I18n::Message::LinearMxpbRegressionFormula
                         : I18n::Message::LinearRegressionFormula;
  }
  int numberOfCoefficients() const override { return 2; }

  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) override;

 protected:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  virtual int slopeCoefficientIndex() const { return 0; }
  virtual int yInterceptCoefficientIndex() const { return 1; }

 private:
  static bool UseMxpbForm() {
    return GlobalPreferences::SharedGlobalPreferences()
               ->regressionAppVariant() ==
           CountryPreferences::RegressionApp::Variant1;
  }

  void privateFit(Store* store, int series, double* modelCoefficients,
                  Poincare::Context* context) override = 0;
};

}  // namespace Regression

#endif
