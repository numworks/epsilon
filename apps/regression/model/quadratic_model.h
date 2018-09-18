#ifndef REGRESSION_QUADRATIC_MODEL_H
#define REGRESSION_QUADRATIC_MODEL_H

#include "model.h"

namespace Regression {

class QuadraticModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  Poincare::Expression simplifiedExpression(double * modelCoefficients, Poincare::Context * context) override;
  I18n::Message formulaMessage() const override { return I18n::Message::QuadraticRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 3; }
  int bannerLinesCount() const override { return 3; }
};

}


#endif
