#ifndef REGRESSION_CUBIC_MODEL_H
#define REGRESSION_CUBIC_MODEL_H

#include "model.h"

namespace Regression {

class CubicModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::CubicRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::Cubic; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 4; }
private:
  Poincare::Expression expression(double * modelCoefficients) override;
};

}


#endif
