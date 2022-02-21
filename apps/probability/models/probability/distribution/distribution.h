#ifndef PROBABILITE_DISTRIBUTION_DISTRIBUTION_H
#define PROBABILITE_DISTRIBUTION_DISTRIBUTION_H

#include "probability/models/inference.h"
#include "probability/models/probability/calculation/discrete_calculation.h"
#include "probability/models/probability/calculation/finite_integral_calculation.h"
#include "probability/models/probability/calculation/left_integral_calculation.h"
#include "probability/models/probability/calculation/right_integral_calculation.h"
#include <new>

namespace Probability {

class Distribution : public Inference {
public:
  Distribution() : m_calculationBuffer(this) {}

  SubApp subApp() const override { return SubApp::Probability; }

  enum class Type : uint8_t{
    // Order matters (displayed order)
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
  static bool Initialize(Distribution * distribution, Type type);
  virtual Type type() const = 0;

  virtual bool isContinuous() const = 0;
  virtual bool isSymmetrical() const = 0;
  virtual double meanAbscissa() { assert(false); return NAN; } // Must be implemented by all symmetrical and continouous distributions.

  // Parameters
  void setParameterAtIndex(double f, int index) override;
  virtual const char * parameterNameAtIndex(int index) const = 0;

  // Evaluation
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double rightIntegralFromAbscissa(double x) const;
  double finiteIntegralBetweenAbscissas(double a, double b) const;
  double cumulativeDistributiveInverseForProbability(double * p) override;
  virtual double rightIntegralInverseForProbability(double * p);
  virtual double evaluateAtDiscreteAbscissa(int k) const;
  constexpr static int k_maxNumberOfOperations = 1000000;
  virtual double defaultComputedValue() const { return 0.0f; }

  Calculation * calculation() { return m_calculationBuffer.calculation(); }

protected:
  static_assert(Poincare::Preferences::VeryLargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_maxProbability = 0.9999995;
  float computeXMin() const override { return -k_displayLeftMarginRatio * computeXMax(); }
  double cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double * p, double ax, double bx);

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

  CalculationBuffer m_calculationBuffer;
};

}

#endif
