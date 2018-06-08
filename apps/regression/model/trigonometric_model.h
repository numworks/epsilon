#ifndef REGRESSION_TRIGONOMETRIC_MODEL_H
#define REGRESSION_TRIGONOMETRIC_MODEL_H

#include "model.h"
#include <poincare/expression_layout.h>

namespace Regression {

class TrigonometricModel : public Model {
public:
  using Model::Model;
  Poincare::ExpressionLayout * layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::TrigonometricRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  bool levelSetAvailable(double * modelCoefficients) const override { return false; }
  double levelSet(double * modelCoefficients, double y) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 3; }
};

}


#endif
