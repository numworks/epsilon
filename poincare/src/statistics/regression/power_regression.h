#ifndef POINCARE_REGRESSION_POWER_REGRESSION_H
#define POINCARE_REGRESSION_POWER_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Internal {

class PowerRegression : public TransformedRegression {
 public:
  Type type() const override { return Type::Power; }

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
};

}  // namespace Poincare::Internal

#endif
