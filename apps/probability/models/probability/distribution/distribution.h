#ifndef PROBABILITE_DISTRIBUTION_DISTRIBUTION_H
#define PROBABILITE_DISTRIBUTION_DISTRIBUTION_H

#include "probability/models/inference.h"

namespace Probability {

class Distribution : public Inference {
public:
  using Inference::Inference;

  enum class Type : uint8_t{
    Binomial,
    Uniform,
    Exponential,
    Normal,
    ChiSquared,
    Student,
    Geometric,
    Poisson,
    Fisher
  };
  static void Initialize(Distribution * distribution, Type type);
  virtual Type type() const = 0;

  virtual bool isContinuous() const = 0;
  virtual bool isSymmetrical() const = 0;
  virtual double meanAbscissa() { assert(false); return NAN; } // Must be implemented by all symmetrical and continouous distributions.

  // Parameters
  void setParameterAtIndex(double f, int index) override;

  // Evaluation
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double rightIntegralFromAbscissa(double x) const;
  double finiteIntegralBetweenAbscissas(double a, double b) const;
  double cumulativeDistributiveInverseForProbability(double * p) override;
  virtual double rightIntegralInverseForProbability(double * p);
  virtual double evaluateAtDiscreteAbscissa(int k) const;
  constexpr static int k_maxNumberOfOperations = 1000000;
  virtual double defaultComputedValue() const { return 0.0f; }

protected:
  static_assert(Poincare::Preferences::VeryLargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_maxProbability = 0.9999995;
  float computeXMin() const override { return -k_displayLeftMarginRatio * computeXMax(); }
  double cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double * p, double ax, double bx);
};

}

#endif
