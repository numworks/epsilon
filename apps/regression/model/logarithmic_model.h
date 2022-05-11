#ifndef REGRESSION_LOGARITHMIC_MODEL_H
#define REGRESSION_LOGARITHMIC_MODEL_H

#include "model.h"

namespace Regression {

class LogarithmicModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::LogarithmicRegressionFormula; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override;
  I18n::Message name() const override { return I18n::Message::Logarithmic; }
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 2; }
protected:
  bool dataSuitableForFit(Store * store, int series) const override;
};

}


#endif
