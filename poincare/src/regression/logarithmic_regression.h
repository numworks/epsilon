#ifndef POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H

#include "transformed_regression.h"

namespace Regression {

class LogarithmicRegression : public TransformedRegression {
 public:
  LogarithmicRegression();
  I18n::Message formulaMessage() const override {
    return I18n::Message::LogarithmicRegressionFormula;
  }
  I18n::Message name() const override { return I18n::Message::Logarithmic; }

 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return false; }
};

}  // namespace Regression

#endif
