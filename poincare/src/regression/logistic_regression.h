#ifndef POINCARE_REGRESSION_LOGISTIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGISTIC_REGRESSION_H

#include "regression.h"

namespace Regression {

class LogisticRegression : public Regression {
 public:
  using Regression::Regression;

  I18n::Message formulaMessage() const override {
    return I18n::Message::LogisticRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Logistic; }
  int numberOfCoefficients() const override { return 3; }

  Poincare::Layout templateLayout() const override;

  double evaluate(double* modelCoefficients, double x) const override;
  double levelSet(double* modelCoefficients, double xMin, double xMax, double y,
                  Poincare::Context* context) override;

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  double partialDerivate(double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  void specializedInitCoefficientsForFit(double* modelCoefficients,
                                         double defaultValue, Store* store,
                                         int series) const override;
};

}  // namespace Regression

#endif
