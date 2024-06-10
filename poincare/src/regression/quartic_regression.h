#ifndef POINCARE_REGRESSION_QUARTIC_REGRESSION_H
#define POINCARE_REGRESSION_QUARTIC_REGRESSION_H

#include "model.h"

namespace Regression {

class QuarticModel : public Model {
 public:
  using Model::Model;
  I18n::Message formulaMessage() const override {
    return I18n::Message::QuarticRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Quartic; }
  int numberOfCoefficients() const override { return 5; }

  Poincare::Layout templateLayout() const override;

  double evaluate(double* modelCoefficients, double x) const override;

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  double partialDerivate(double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Regression

#endif
