#ifndef POINCARE_REGRESSION_POWER_REGRESSION_H
#define POINCARE_REGRESSION_POWER_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class PowerRegression : public TransformedRegression {
 public:
  const char* formula() const override { return "y=a·x^b"; }
  Poincare::Layout templateLayout() const override;

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return true; }
};

}  // namespace Poincare::Regression

#endif
