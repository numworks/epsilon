#ifndef POINCARE_REGRESSION_POWER_REGRESSION_H
#define POINCARE_REGRESSION_POWER_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Regression {

class PowerRegression : public TransformedRegression {
 public:
  Type type() const override { return Type::Power; }

 private:
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
};

}  // namespace Poincare::Regression

#endif
