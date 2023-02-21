#ifndef REGRESSION_POWER_MODEL_H
#define REGRESSION_POWER_MODEL_H

#include "model.h"

namespace Regression {

class PowerModel : public Model {
 public:
  using Model::Model;
  I18n::Message formulaMessage() const override {
    return I18n::Message::PowerRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Power; }
  int numberOfCoefficients() const override { return 2; }

  Poincare::Layout templateLayout() const override;

  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) override;

 private:
  Poincare::Expression privateExpression(
      double* modelCoefficients) const override;
  double partialDerivate(double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  void privateFit(Store* store, int series, double* modelCoefficients,
                  Poincare::Context* context) override;
  bool dataSuitableForFit(Store* store, int series) const override;
};

}  // namespace Regression

#endif
