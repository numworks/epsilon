#ifndef REGRESSION_CUBIC_MODEL_H
#define REGRESSION_CUBIC_MODEL_H

#include "model.h"

namespace Regression {

class CubicModel : public Model {
public:
  using Model::Model;
  I18n::Message name() const override { return I18n::Message::Cubic; }
  I18n::Message formulaMessage() const override { return I18n::Message::CubicRegressionFormula; }
  int numberOfCoefficients() const override { return 4; }

  Poincare::Layout templateLayout() const override;

  double evaluate(double * modelCoefficients, double x) const override;

private:
  Poincare::Expression privateExpression(double * modelCoefficients) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
};

}


#endif
