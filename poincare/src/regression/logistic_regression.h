#ifndef POINCARE_REGRESSION_LOGISTIC_REGRESSION_H
#define POINCARE_REGRESSION_LOGISTIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class LogisticRegression : public Regression {
 public:
  using Regression::Regression;

  Type type() const override { return Type::Logistic; }

  double evaluate(const double* modelCoefficients, double x) const override;
  double levelSet(const double* modelCoefficients, double xMin, double xMax,
                  double y, Poincare::Context* context) const override;

 private:
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  void specializedInitCoefficientsForFit(double* modelCoefficients,
                                         double defaultValue,
                                         const Series* series) const override;
};

}  // namespace Poincare::Regression

#endif
