#ifndef REGRESSION_QUADRATIC_MODEL_H
#define REGRESSION_QUADRATIC_MODEL_H

#include "model.h"

namespace Regression {

class QuadraticModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::QuadraticRegressionFormula; }
  int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const override;
  I18n::Message name() const override { return I18n::Message::Quadratic; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 3; }
private:
  Poincare::Expression expression(double * modelCoefficients) override;
};

}


#endif
