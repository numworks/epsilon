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

  constexpr static int k_numberOfCoefficients = 4;

 private:
  static constexpr size_t k_defaultParametersIterations = 10;

  double privateEvaluate(const CoefficientsType& modelCoefficients,
                         double x) const override;

  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const CoefficientsType& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  CoefficientsType specializedInitCoefficientsForFit(
      double defaultValue, size_t attemptNumber,
      const Series* series) const override;
  void uniformizeCoefficientsFromFit(
      CoefficientsType& modelCoefficients) const override;
};

}  // namespace Poincare::Regression

#endif
