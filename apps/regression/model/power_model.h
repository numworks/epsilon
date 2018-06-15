#ifndef REGRESSION_POWER_MODEL_H
#define REGRESSION_POWER_MODEL_H

#include "model.h"
#include <poincare/expression_layout.h>

namespace Regression {

class PowerModel : public Model {
public:
  using Model::Model;
  Poincare::ExpressionLayout * layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::PowerRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return 2; }
protected:
  virtual bool dataSuitableForFit(Store * store, int series) const override;
};

}


#endif
