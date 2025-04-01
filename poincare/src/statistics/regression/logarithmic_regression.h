#ifndef POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGARITHMIC_REGRESSION_H

#include "transformed_regression.h"

namespace Poincare::Internal {

class LogarithmicRegression : public TransformedRegression {
 public:
  Type type() const override { return Type::Logarithmic; }

 private:
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
};

}  // namespace Poincare::Internal

#endif
