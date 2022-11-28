#ifndef REGRESSION_PROPORTIONAL_MODEL_H
#define REGRESSION_PROPORTIONAL_MODEL_H

#include "model.h"

namespace Regression {

class ProportionalModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { return I18n::Message::ProportionalRegressionFormula; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override;
  I18n::Message name() const override { return I18n::Message::Proportional; }
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 1; }
};

}


#endif
