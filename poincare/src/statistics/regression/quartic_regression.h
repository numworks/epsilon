#ifndef POINCARE_REGRESSION_QUARTIC_REGRESSION_H
#define POINCARE_REGRESSION_QUARTIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Internal {

class QuarticRegression : public Regression {
 public:
  Type type() const override { return Type::Quartic; }

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
