#ifndef REGRESSION_TRIGONOMETRIC_MODEL_H
#define REGRESSION_TRIGONOMETRIC_MODEL_H

#include "model.h"

namespace Regression {

class TrigonometricModel : public Model {
public:
  using Model::Model;
  I18n::Message formulaMessage() const override { return I18n::Message::TrigonometricRegressionFormula; }
  I18n::Message name() const override { return I18n::Message::Trigonometrical; }
  int numberOfCoefficients() const override { return k_numberOfCoefficients; }

  Poincare::Expression expression(double * modelCoefficients) const override;

  double evaluate(double * modelCoefficients, double x) const override;

private:
  constexpr static int k_numberOfCoefficients = 4;
  double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const override;
  void specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store, int series) const override;
  void uniformizeCoefficientsFromFit(double * modelCoefficients) const override;
};

}

#endif
