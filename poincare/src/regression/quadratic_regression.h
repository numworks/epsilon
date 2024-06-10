#ifndef POINCARE_REGRESSION_QUADRATIC_REGRESSION_H
#define POINCARE_REGRESSION_QUADRATIC_REGRESSION_H

#include "regression.h"

namespace Regression {

class QuadraticRegression : public Regression {
 public:
  using Regression::Regression;
  I18n::Message formulaMessage() const override {
    return I18n::Message::QuadraticRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Quadratic; }
  int numberOfCoefficients() const override { return 3; }

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
