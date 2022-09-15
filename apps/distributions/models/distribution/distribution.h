#ifndef PROBABILITE_DISTRIBUTION_DISTRIBUTION_H
#define PROBABILITE_DISTRIBUTION_DISTRIBUTION_H

#include <apps/shared/inference.h>
#include "distributions/models/calculation/discrete_calculation.h"
#include "distributions/models/calculation/finite_integral_calculation.h"
#include "distributions/models/calculation/left_integral_calculation.h"
#include "distributions/models/calculation/right_integral_calculation.h"
#include <poincare/distribution.h>
#include <new>

namespace Distributions {

class Distribution : public Shared::Inference {
public:
  Distribution(Poincare::Distribution::Type type) : m_calculationBuffer(this), m_distribution(Poincare::Distribution::Get(type)) {}

  static bool Initialize(Distribution * distribution, Poincare::Distribution::Type type);
  Poincare::Distribution::Type type() const { return m_distribution->type(); };

  virtual double meanAbscissa() { assert(false); return NAN; } // Must be implemented by all symmetrical and continouous distributions.
  bool isContinuous() const { return m_distribution->isContinuous(); }
  bool isSymmetrical() const { return m_distribution->isSymmetrical(); }

  // Parameters
  void setParameterAtIndex(double f, int index) override;
  virtual const char * parameterNameAtIndex(int index) const = 0;

  // Evaluation
  float evaluateAtAbscissa(float x) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double rightIntegralFromAbscissa(double x) const;
  double finiteIntegralBetweenAbscissas(double a, double b) const;
  double cumulativeDistributiveInverseForProbability(double p) const override;
  virtual double rightIntegralInverseForProbability(double p) const;
  virtual double evaluateAtDiscreteAbscissa(int k) const;
  constexpr static int k_maxNumberOfOperations = 1000000;
  virtual double defaultComputedValue() const { return 0.0f; }

  Calculation * calculation() { return m_calculationBuffer.calculation(); }

protected:
  static_assert(Poincare::Preferences::VeryLargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_maxProbability = 0.9999995;
  float computeXMin() const override { return -k_displayLeftMarginRatio * computeXMax(); }

  union CalculationBuffer {
  public:
    CalculationBuffer(Distribution * distribution) { new (&m_leftIntegralCalculation) LeftIntegralCalculation(distribution); }
    ~CalculationBuffer() { calculation()->~Calculation(); }
    // Rule of 5
    CalculationBuffer(const CalculationBuffer& other) = delete;
    CalculationBuffer(CalculationBuffer&& other) = delete;
    CalculationBuffer& operator=(const CalculationBuffer& other) = delete;
    CalculationBuffer& operator=(CalculationBuffer&& other) = delete;

    Calculation * calculation() {
      return reinterpret_cast<Calculation *>(this);
    }
  private:
    DiscreteCalculation m_discreteCalculation;
    FiniteIntegralCalculation m_finiteIntegralCalculation;
    LeftIntegralCalculation m_leftIntegralCalculation;
    RightIntegralCalculation m_rightIntegralCalculation;
  };

  virtual const double * constParametersArray() const = 0;

  CalculationBuffer m_calculationBuffer;
  const Poincare::Distribution * m_distribution;
};

}

#endif
