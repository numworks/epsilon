#ifndef REGRESSION_TRIGONOMETRIC_MODEL_H
#define REGRESSION_TRIGONOMETRIC_MODEL_H

#include "model.h"

namespace Regression {

class TrigonometricModel : public Model {
public:
  using Model::Model;
  Poincare::Layout layout() override;
  I18n::Message formulaMessage() const override { return I18n::Message::TrigonometricRegressionFormula; }
  double evaluate(double * modelCoefficients, double x) const override;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  int numberOfCoefficients() const override { return k_numberOfCoefficients; }
  int bannerLinesCount() const override { return 4; }
private:
  static constexpr int k_numberOfCoefficients = 4;
  void specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const override;
  Poincare::Expression expression(double * modelCoefficients) override;
};

}

#endif
