#ifndef POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class LogarithmicRegression : public TransformedRegression {
 public:
  Type type() const override { return Type::Logarithmic; }

 private:
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  bool applyLnOnX() const override { return true; }
  bool applyLnOnY() const override { return false; }
};

}  // namespace Poincare::Regression

#endif
