#ifndef POINCARE_REGRESSION_CUBIC_REGRESSION_H
#define POINCARE_REGRESSION_CUBIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Internal {

class CubicRegression : public Regression {
 public:
  Type type() const override { return Type::Cubic; }

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
