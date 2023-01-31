#ifndef REGRESSION_LOGARITHMIC_MODEL_H
#define REGRESSION_LOGARITHMIC_MODEL_H

#include "model.h"

namespace Regression {

class LogarithmicModel : public Model {
public:
  using Model::Model;

  I18n::Message formulaMessage() const override { return I18n::Message::LogarithmicRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::Logarithmic; }
  int numberOfCoefficients() const override { return 2; }

  Poincare::Expression expression(double * modelCoefficients) const override;

  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;

private:
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  bool dataSuitableForFit(Store * store, int series) const override;
};

}


#endif
