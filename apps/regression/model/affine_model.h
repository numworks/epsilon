#ifndef REGRESSION_AFFINE_MODEL_H
#define REGRESSION_AFFINE_MODEL_H

#include "model.h"

namespace Regression {

// This is a pure virtual class that factorises all regression models that compute an affine function (linear model and median-median model)
class AffineModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::LinearRegressionFormula; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override;
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 2; }
private:
  virtual void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override = 0;
};

}


#endif
