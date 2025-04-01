#ifndef POINCARE_REGRESSION_LOGISTIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGISTIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Internal {

class LogisticRegression : public Regression {
 public:
  Type type() const override { return Type::Logistic; }

  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 private:
  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;
  Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const Coefficients& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  Coefficients specializedInitCoefficientsForFit(
      double defaultValue, size_t /* attemptNumber */,
      const Series* s = nullptr) const override;
};

}  // namespace Poincare::Internal

#endif
