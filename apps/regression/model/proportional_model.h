#ifndef REGRESSION_PROPORTIONAL_MODEL_H
#define REGRESSION_PROPORTIONAL_MODEL_H

#include "model.h"

namespace Regression {

class ProportionalModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { return I18n::Message::ProportionalRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::Proportional; }
  int numberOfCoefficients() const override { return 1; }

  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context) override;

private:
  Poincare::Expression privateExpression(double * modelCoefficients) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
};

}


#endif
