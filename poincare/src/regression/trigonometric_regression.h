#ifndef POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H
#define POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H

#include "regression.h"

namespace Poincare::Regression {

class TrigonometricRegression : public Regression {
 public:
  using Regression::Regression;

  constexpr TrigonometricRegression(
      size_t initialParametersIterations = k_defaultParametersIterations)
      : Regression(initialParametersIterations, k_lowerRegressionScoreFactor) {}

  Type type() const override { return Type::Trigonometric; }

  constexpr static int k_numberOfCoefficients = 4;

 private:
  /* Because the trigonometric regression is very sensitive to the initial
   * parameters, the fit algorithm is called in a loop several times, with
   * different initial parameter guesses. */
  static constexpr size_t k_defaultParametersIterations = 10;

  /* We want to bias the best coefficient selection to favor lower frequencies.
   * If two models give a similar score (in terms of residual standard
   * deviation), we want to select the one with the lowest frequency. The
   * initial frequency parameter (the "b" coefficient) guess goes from a lower
   * frequency guess to a higher frequency guess. We make sure that a high
   * frequency will only be selected as the best model if it has a better score
   * than the previous best score (obtained with a lower frequency) multiplied
   * by a certain factor below 1.  */
  static constexpr double k_lowerRegressionScoreFactor = 0.9;

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
