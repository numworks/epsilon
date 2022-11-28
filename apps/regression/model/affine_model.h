#ifndef REGRESSION_AFFINE_MODEL_H
#define REGRESSION_AFFINE_MODEL_H

#include "model.h"
#include <apps/global_preferences.h>

namespace Regression {

// This is a pure virtual class that factorises all regression models that compute an affine function (linear model and median-median model)
class AffineModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { return UseMxpbForm() ?  I18n::Message::LinearMxpbRegressionFormula : I18n::Message::LinearRegressionFormula; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override;
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 2; }

protected:
  virtual int slopeCoefficientIndex() const { return 0; }
  virtual int yInterceptCoefficientIndex() const { return 1; }
  virtual const char * equationTemplate() const { return "%*.*ed·x%+*.*ed"; }

private:
  static bool UseMxpbForm() { return GlobalPreferences::sharedGlobalPreferences()->regressionModelOrder() == CountryPreferences::RegressionModelOrder::Variant1; }

  void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override = 0;
};

}


#endif
