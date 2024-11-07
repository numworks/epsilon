#ifndef POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H
#define POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class TrigonometricRegression : public Regression {
 public:
  using Regression::Regression;

  constexpr TrigonometricRegression(
      size_t initialParametersIterations = k_defaultParametersIterations)
      : Regression(initialParametersIterations) {}

  Type type() const override { return Type::Trigonometric; }
  double evaluate(const double* modelCoefficients, double x) const override;

  constexpr static int k_numberOfCoefficients = 4;

 private:
  static constexpr size_t k_defaultParametersIterations = 10;

  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const double* modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  CoefficientsType specializedInitCoefficientsForFit(
      double defaultValue, size_t attemptNumber,
      const Series* series) const override;
  void uniformizeCoefficientsFromFit(double* modelCoefficients) const override;
};

}  // namespace Poincare::Regression

#endif
