#ifndef PROBABILITY_MODELS_BUFFER_H
#define PROBABILITY_MODELS_BUFFER_H

#include <new>

#include "probability/calculation/discrete_calculation.h"
#include "probability/calculation/finite_integral_calculation.h"
#include "probability/calculation/left_integral_calculation.h"
#include "probability/calculation/right_integral_calculation.h"
#include "probability/distribution/binomial_distribution.h"
#include "probability/distribution/chi_squared_distribution.h"
#include "probability/distribution/exponential_distribution.h"
#include "probability/distribution/fisher_distribution.h"
#include "probability/distribution/geometric_distribution.h"
#include "probability/distribution/normal_distribution.h"
#include "probability/distribution/poisson_distribution.h"
#include "probability/distribution/student_distribution.h"
#include "probability/distribution/uniform_distribution.h"
#include "probability/helpers.h"
#include "statistic/goodness_test.h"
#include "statistic/homogeneity_test.h"
#include "statistic/hypothesis_params.h"
#include "statistic/one_mean_t_interval.h"
#include "statistic/one_mean_t_test.h"
#include "statistic/one_mean_z_interval.h"
#include "statistic/one_mean_z_test.h"
#include "statistic/one_proportion_z_interval.h"
#include "statistic/one_proportion_z_test.h"
#include "statistic/pooled_two_means_t_interval.h"
#include "statistic/pooled_two_means_t_test.h"
#include "statistic/two_means_t_interval.h"
#include "statistic/two_means_t_test.h"
#include "statistic/two_means_z_interval.h"
#include "statistic/two_means_z_test.h"
#include "statistic/two_proportions_z_interval.h"
#include "statistic/two_proportions_z_test.h"

namespace Probability {

// Buffers for dynamic allocation

union DistributionBuffer {
public:
  DistributionBuffer() { new (&m_binomialDistribution) BinomialDistribution(); }
  ~DistributionBuffer() { distribution()->~Distribution(); }
  // Rule of 5
  DistributionBuffer(const DistributionBuffer& other) = delete;
  DistributionBuffer(DistributionBuffer&& other) = delete;
  DistributionBuffer& operator=(const DistributionBuffer& other) = delete;
  DistributionBuffer& operator=(DistributionBuffer&& other) = delete;

  Distribution * distribution() {
    return reinterpret_cast<Distribution *>(this);
  }
private:
  BinomialDistribution m_binomialDistribution;
  UniformDistribution m_uniformDistribution;
  ExponentialDistribution m_exponentDistribution;
  NormalDistribution m_normalDistribution;
  ChiSquaredDistribution m_chiSquaredDistribution;
  StudentDistribution m_studentDistribution;
  GeometricDistribution m_geometricDistribution;
  PoissonDistribution m_poissonDistribution;
  FisherDistribution m_fisherDistribution;
};

union CalculationBuffer {
public:
  CalculationBuffer(Distribution * distribution) { new (&m_discreteCalculation) DiscreteCalculation(distribution); }
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

#warning Test on emscripten for alignment

class ProbabilityBuffer {
public:
  ProbabilityBuffer() :
    m_distributionBuffer(),
    m_calculationBuffer(distribution()) {}
  Distribution * distribution() {
    return m_distributionBuffer.distribution();
  }
  Calculation * calculation() {
    return m_calculationBuffer.calculation();
  }

private:
  DistributionBuffer m_distributionBuffer;
  CalculationBuffer m_calculationBuffer;
};

union StatisticBuffer {
public:
  StatisticBuffer() { new (&m_oneMeanTInterval) OneMeanTInterval(); }
  ~StatisticBuffer() { statistic()->~Statistic(); }
  // Rule of 5
  StatisticBuffer(const StatisticBuffer& other) = delete;
  StatisticBuffer(StatisticBuffer&& other) = delete;
  StatisticBuffer& operator=(const StatisticBuffer& other) = delete;
  StatisticBuffer& operator=(StatisticBuffer&& other) = delete;

  Statistic * statistic() {
    return reinterpret_cast<Statistic *>(this);
  }
private:
  OneMeanTInterval m_oneMeanTInterval;
  OneMeanTTest m_oneMeanTTest;
  OneMeanZInterval m_oneMeanZInterval;
  OneMeanZTest m_oneMeanZTest;
  OneProportionZInterval m_oneProportionZInterval;
  OneProportionZTest m_oneProportionZTest;
  PooledTwoMeansTInterval m_pooledTwoMeansTInterval;
  PooledTwoMeansTTest m_pooledTwoMeansTTest;
  TwoMeansTInterval m_twoMeansTInterval;
  TwoMeansTTest m_twoMeansTTest;
  TwoMeansZInterval m_twoMeansZInterval;
  TwoMeansZTest m_twoMeansZTest;
  TwoProportionsZInterval m_twoProportionsZInterval;
  TwoProportionsZTest m_twoProportionsZTest;
  GoodnessTest m_goodnessTest;
  HomogeneityTest m_homogeneityTest;
};

#warning Test on emscripten for alignment

union ModelBuffer {
public:
  ModelBuffer() { new (&m_probabilityBuffer) ProbabilityBuffer(); }
  ~ModelBuffer() { inference()->~Inference(); }
  Inference * inference() { return reinterpret_cast<Inference *>(this); }
  Distribution * distribution() { return m_probabilityBuffer.distribution(); }
  Calculation * calculation() { return m_probabilityBuffer.calculation(); }
  Statistic * statistic() { return m_statisticBuffer.statistic(); }

private:
  ProbabilityBuffer m_probabilityBuffer;
  StatisticBuffer m_statisticBuffer;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_BUFFER_H */
