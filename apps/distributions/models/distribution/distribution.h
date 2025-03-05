#ifndef PROBABILITE_DISTRIBUTION_DISTRIBUTION_H
#define PROBABILITE_DISTRIBUTION_DISTRIBUTION_H

#include <apps/shared/inference.h>
#include <poincare/statistics/distribution.h>

#include "distributions/models/calculation/discrete_calculation.h"
#include "distributions/models/calculation/finite_integral_calculation.h"
#include "distributions/models/calculation/left_integral_calculation.h"
#include "distributions/models/calculation/right_integral_calculation.h"

namespace Distributions {

class Distribution : public Shared::Inference {
 public:
  Distribution(Poincare::Distribution::Type type)
      : m_calculationBuffer(),
        m_distribution(type),
        m_indexOfUninitializedParameter(-1) {
    m_calculationBuffer.init(this);
  }

  static bool Initialize(Distribution* distribution,
                         Poincare::Distribution::Type type);
  Poincare::Distribution::Type type() const { return m_distribution.type(); };

  bool isContinuous() const { return m_distribution.isContinuous(); }
  bool isSymmetrical() const { return m_distribution.isSymmetrical(); }
  double meanAbscissa() const {
    return m_distribution.meanAbscissa(constParametersArray());
  }

  // Parameters
  int numberOfParameters() override {
    return m_distribution.numberOfParameters();
  }
  const char* parameterNameAtIndex(int index) const {
    return m_distribution.parameterNameAtIndex(index);
  }
  double defaultParameterAtIndex(int index) const {
    return m_distribution.defaultParameterAtIndex(index);
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
  void setParameterAtIndex(double f, int index) override;

  // Unknown parameter
  virtual bool canHaveUninitializedParameter() const { return false; }
  int uninitializedParameterIndex() const {
    return m_indexOfUninitializedParameter;
  }
  bool allParametersAreInitialized() const {
    return m_indexOfUninitializedParameter == k_allParametersAreInitialized;
  }

  // Evaluation
  /* This does computation in double and then converts to float.
   * TODO: Should this just return double? */
  float evaluateAtAbscissa(float x) const override final;
  double cumulativeDistributiveFunctionAtAbscissa(
      double x) const override final;
  double rightIntegralFromAbscissa(double x) const;
  double finiteIntegralBetweenAbscissas(double a, double b) const;
  double cumulativeDistributiveInverseForProbability(
      double p) const override final;
  virtual double rightIntegralInverseForProbability(double p) const;
  double evaluateAtDiscreteAbscissa(int k) const;
  constexpr static int k_maxNumberOfOperations = 1000000;
  virtual double defaultComputedValue() const { return 0.0f; }
  void computeUnknownParameterForProbabilityAndBound(double probability,
                                                     double bound,
                                                     bool isUpperBound);

  Calculation* calculation() { return m_calculationBuffer.calculation(); }

  using ParametersArray = Poincare::Distribution::ParametersArray<double>;
  const ParametersArray constParametersArray() const {
    ParametersArray array;
    const double* paramsArray =
        const_cast<Distribution*>(this)->parametersArray();
    std::copy(paramsArray,
              paramsArray + Poincare::Distribution::k_maxNumberOfParameters,
              array.data());
    return array;
  }

 protected:
  static_assert(Poincare::Preferences::VeryLargeNumberOfSignificantDigits == 7,
                "k_maxProbability is ill-defined compared to "
                "LargeNumberOfSignificantDigits");
  constexpr static double k_maxProbability = 0.9999995;
  constexpr static int k_allParametersAreInitialized = -1;

  float computeXMax() const override final { return computeXExtremum(false); }
  float computeXMin() const override final { return computeXExtremum(true); }
  float computeXExtremum(bool min) const;

  virtual float privateComputeXMax() const = 0;
  virtual float privateComputeXMin() const {
    return -k_displayLeftMarginRatio * privateComputeXMax();
  }
  void setParameterAtIndexWithoutComputingCurveViewRange(double x, int index);

  union CalculationBuffer {
   public:
    CalculationBuffer() {}
    ~CalculationBuffer() { calculation()->~Calculation(); }
    // Rule of 5
    CalculationBuffer(const CalculationBuffer& other) = delete;
    CalculationBuffer(CalculationBuffer&& other) = delete;
    CalculationBuffer& operator=(const CalculationBuffer& other) = delete;
    CalculationBuffer& operator=(CalculationBuffer&& other) = delete;

    void init(Distribution* distribution) {
      new (&m_leftIntegralCalculation) LeftIntegralCalculation(distribution);
    }

    Calculation* calculation() { return reinterpret_cast<Calculation*>(this); }

   private:
    DiscreteCalculation m_discreteCalculation;
    FiniteIntegralCalculation m_finiteIntegralCalculation;
    LeftIntegralCalculation m_leftIntegralCalculation;
    RightIntegralCalculation m_rightIntegralCalculation;
  };

  CalculationBuffer m_calculationBuffer;
  const Poincare::Distribution m_distribution;
  // Used if one of the parameters is not input by the user
  int m_indexOfUninitializedParameter;
};

}  // namespace Distributions

#endif
