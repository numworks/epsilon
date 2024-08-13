#ifndef POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H
#define POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class TrigonometricRegression : public Regression {
 public:
  using Regression::Regression;

  Type type() const override { return Type::Trigonometric; }
  double evaluate(const double* modelCoefficients, double x) const override;

 private:
  constexpr static int k_numberOfCoefficients = 4;
  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  void specializedInitCoefficientsForFit(double* modelCoefficients,
                                         double defaultValue,
                                         const Series* series) const override;
  void uniformizeCoefficientsFromFit(double* modelCoefficients) const override;
};

}  // namespace Poincare::Regression

#endif
