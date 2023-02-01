#ifndef REGRESSION_QUARTIC_MODEL_H
#define REGRESSION_QUARTIC_MODEL_H

#include "model.h"

namespace Regression {

class QuarticModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { return I18n::Message::QuarticRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::Quartic; }
  int numberOfCoefficients() const override { return 5; }

  Poincare::Layout templateLayout() const override;
  Poincare::Expression expression(double * modelCoefficients) const override;

  double evaluate(double * modelCoefficients, double x) const override;

private:
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
};

}


#endif
