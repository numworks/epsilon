#ifndef POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H
#define POINCARE_REGRESSION_TRIGONOMETRIC_REGRESSION_H

#include <poincare/preferences.h>

#include "regression.h"

namespace Poincare::Regression {

class TrigonometricRegression : public Regression {
 public:
  constexpr TrigonometricRegression(
      Preferences::AngleUnit angleUnit,
      size_t initialParametersIterations = k_defaultParametersIterations)
      : Regression(initialParametersIterations), m_angleUnit(angleUnit) {}

  Type type() const override { return Type::Trigonometric; }

  constexpr static int k_numberOfCoefficients = 4;

 private:
  /* Because the trigonometric regression is very sensitive to the initial
   * parameters, the fit algorithm is called in a loop several times, with
   * different initial parameter guesses. */
  static constexpr size_t k_defaultParametersIterations = 7;

  /* Trigonometric regression is attempted several times with different initial
   * parameters. The more sensitive initial parameter is the frequency. The
   * different initial frequencies are computed by multiplying a "base" guess
   * frequency with a scaling factor. The base frequency is scaled up and down
   * by powers of the following factor. Scaling with powers of 2 is a good
   * choice for trigonometric functions, to explore harmonics of the initially
   * guessed frequency. However it is better to have a finer grain in some
   * cases, so we choose √2 as the multiplication factor. */
  static constexpr double k_frequencyMultiplicationFactor = M_SQRT2;

  double privateEvaluate(const Coefficients& modelCoefficients,
                         double x) const override;

  Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const override;
  double partialDerivate(const Coefficients& modelCoefficients,
                         int derivateCoefficientIndex, double x) const override;
  Coefficients specializedInitCoefficientsForFit(
      double defaultValue, size_t attemptNumber,
      const Series* series) const override;
  void uniformizeCoefficientsFromFit(
      Coefficients& modelCoefficients) const override;

  bool isRegressionBetter(
      double residualStandardDeviation1, double residualStandardDeviation2,
      const Regression::Coefficients& modelCoefficients1,
      const Regression::Coefficients& modelCoefficients2) const override;

  double toRadiansCoeff() const;

  Preferences::AngleUnit m_angleUnit;
};

}  // namespace Poincare::Regression

#endif
