#ifndef POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class LogarithmicRegression : public TransformedRegression {
 private:
  const char* formula() const override { return "y=a+b·ln(x)"; }
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return false; }
};

}  // namespace Poincare::Regression

#endif
