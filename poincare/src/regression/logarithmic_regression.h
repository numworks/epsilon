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
};

}  // namespace Poincare::Regression

#endif
