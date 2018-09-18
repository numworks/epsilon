#ifndef REGRESSION_TRIGONOMETRIC_MODEL_H
#define REGRESSION_TRIGONOMETRIC_MODEL_H

#include "model.h"

namespace Regression {

class TrigonometricModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  Poincare::Expression simplifiedExpression(double * modelCoefficients, Poincare::Context * context) override;
  I18n::Message formulaMessage() const override { return I18n::Message::TrigonometricRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 4; }
  int bannerLinesCount() const override { return 4; }
};

}


#endif
