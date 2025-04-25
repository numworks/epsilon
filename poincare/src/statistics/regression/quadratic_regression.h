#ifndef POINCARE_REGRESSION_QUADRATIC_REGRESSION_H
#define POINCARE_REGRESSION_QUADRATIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Internal {

class QuadraticRegression : public Regression {
 public:
  Type type() const override { return Type::Quadratic; }

 private:
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const Coefficients& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  void offsetCoefficients(Coefficients& modelCoefficients,
                          const OffsetSeries* series) const override;
};

}  // namespace Poincare::Internal

#endif
