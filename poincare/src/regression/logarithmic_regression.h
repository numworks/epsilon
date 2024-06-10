#ifndef POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class LogarithmicRegression : public TransformedRegression {
 private:
  Poincare::UserExpression privateExpression(
      double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return false; }
};

}  // namespace Poincare::Regression

#endif
