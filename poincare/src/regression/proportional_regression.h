#ifndef POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H
#define POINCARE_REGRESSION_PROPORTIONAL_REGRESSION_H

#include "regression.h"

namespace Regression {

class ProportionalRegression : public Regression {
 public:
  using Regression::Regression;
  I18n::Message formulaMessage() const override {
    return I18n::Message::ProportionalRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Proportional; }
  int numberOfCoefficients() const override { return 1; }

  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) override;

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  double partialDerivate(double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
};

}  // namespace Regression

#endif
