#ifndef INFERENCE_MODELS_BUFFER_H
#define INFERENCE_MODELS_BUFFER_H

#include <new>

#include "probability/distribution/binomial_distribution.h"
#include "probability/distribution/chi_squared_distribution.h"
#include "probability/distribution/exponential_distribution.h"
#include "probability/distribution/fisher_distribution.h"
#include "probability/distribution/geometric_distribution.h"
#include "probability/distribution/hypergeometric_distribution.h"
#include "probability/distribution/normal_distribution.h"
#include "probability/distribution/poisson_distribution.h"
#include "probability/distribution/student_distribution.h"
#include "probability/distribution/uniform_distribution.h"
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

namespace Inference {

// Buffers for dynamic allocation

union DistributionBuffer {
public:
  DistributionBuffer() {
    new (&m_binomialDistribution) BinomialDistribution();
    distribution()->calculation()->compute(0);
  }
  ~DistributionBuffer() { distribution()->~Distribution(); }
  // Rule of 5
  DistributionBuffer(const DistributionBuffer& other) = delete;
  DistributionBuffer(DistributionBuffer&& other) = delete;
  DistributionBuffer& operator=(const DistributionBuffer& other) = delete;
  DistributionBuffer& operator=(DistributionBuffer&& other) = delete;

  Distribution * distribution() { return reinterpret_cast<Distribution *>(this); }

private:
  BinomialDistribution m_binomialDistribution;
  UniformDistribution m_uniformDistribution;
  ExponentialDistribution m_exponentDistribution;
  NormalDistribution m_normalDistribution;
  ChiSquaredDistribution m_chiSquaredDistribution;
  StudentDistribution m_studentDistribution;
  GeometricDistribution m_geometricDistribution;
  HypergeometricDistribution m_hypergeometricDistribution;
  PoissonDistribution m_poissonDistribution;
  FisherDistribution m_fisherDistribution;
};

union StatisticBuffer {
public:
  StatisticBuffer() {
    new (&m_oneMeanTInterval) OneMeanTInterval();
    statistic()->initParameters();
  }
  ~StatisticBuffer() { statistic()->~Statistic(); }
  // Rule of 5
  StatisticBuffer(const StatisticBuffer& other) = delete;
  StatisticBuffer(StatisticBuffer&& other) = delete;
  StatisticBuffer& operator=(const StatisticBuffer& other) = delete;
  StatisticBuffer& operator=(StatisticBuffer&& other) = delete;

  Statistic * statistic() { return reinterpret_cast<Statistic *>(this); }
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

union ModelBuffer {
public:
  ModelBuffer() { new (&m_distributionBuffer) DistributionBuffer(); }
  ~ModelBuffer() { inference()->~Inference(); }
  Inference * inference() { return reinterpret_cast<Inference *>(this); }
  Distribution * distribution() { return m_distributionBuffer.distribution(); }
  Calculation * calculation() { return distribution()->calculation(); }
  Statistic * statistic() { return m_statisticBuffer.statistic(); }

private:
  DistributionBuffer m_distributionBuffer;
  StatisticBuffer m_statisticBuffer;
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_BUFFER_H */
