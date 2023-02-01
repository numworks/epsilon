#ifndef REGRESSION_QUADRATIC_MODEL_H
#define REGRESSION_QUADRATIC_MODEL_H

#include "model.h"

namespace Regression {

class QuadraticModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { return I18n::Message::QuadraticRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::Quadratic; }
  int numberOfCoefficients() const override { return 3; }

  Poincare::Layout templateLayout() const override;

  double evaluate(double * modelCoefficients, double x) const override;

private:
  Poincare::Expression privateExpression(double * modelCoefficients) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
};

}


#endif
