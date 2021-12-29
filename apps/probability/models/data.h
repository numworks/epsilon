#ifndef PROBABILITY_MODELS_DATA_H
#define PROBABILITY_MODELS_DATA_H

/*
 * This header defines the structures (enums/structs/typedefs/classes)
 * to store the data for the Distribution App.
 * The entrypoint is the Data class, which is meant to contain all the data,
 * and provide convenient methods to access it.
 */
#include <new>

#include "calculation/discrete_calculation.h"
#include "calculation/finite_integral_calculation.h"
#include "calculation/left_integral_calculation.h"
#include "calculation/right_integral_calculation.h"
#include "data_enums.h"
#include "distribution/binomial_distribution.h"
#include "distribution/chi_squared_distribution.h"
#include "distribution/exponential_distribution.h"
#include "distribution/fisher_distribution.h"
#include "distribution/geometric_distribution.h"
#include "distribution/normal_distribution.h"
#include "distribution/poisson_distribution.h"
#include "distribution/student_distribution.h"
#include "distribution/uniform_distribution.h"
#include "probability/helpers.h"
#include "probability/models/hypothesis_params.h"
#include "probability/models/statistic/chi2_statistic.h"
#include "probability/models/statistic/goodness_statistic.h"
#include "probability/models/statistic/homogeneity_statistic.h"
#include "probability/models/statistic/one_mean_t_statistic.h"
#include "probability/models/statistic/one_mean_z_statistic.h"
#include "probability/models/statistic/one_proportion_statistic.h"
#include "probability/models/statistic/pooled_two_means_statistic.h"
#include "probability/models/statistic/statistic.h"
#include "probability/models/statistic/two_means_t_statistic.h"
#include "probability/models/statistic/two_means_z_statistic.h"
#include "probability/models/statistic/two_proportions_statistic.h"

namespace Probability {
namespace Data {

// Buffers for dynamic allocation

static constexpr int distributionSizes[9] = {sizeof(ChiSquaredDistribution),
                                             sizeof(ExponentialDistribution),
                                             sizeof(GeometricDistribution),
                                             sizeof(PoissonDistribution),
                                             sizeof(StudentDistribution),
                                             sizeof(BinomialDistribution),
                                             sizeof(FisherDistribution),
                                             sizeof(NormalDistribution),
                                             sizeof(UniformDistribution)};

static constexpr int maxDistributionSize = arrayMax(distributionSizes);
typedef char DistributionBuffer[maxDistributionSize];

static constexpr int calculationSizes[4] = {sizeof(DiscreteCalculation),
                                            sizeof(FiniteIntegralCalculation),
                                            sizeof(LeftIntegralCalculation),
                                            sizeof(RightIntegralCalculation)};
static constexpr int maxCalculationSize = arrayMax(calculationSizes);
typedef char CalculationBuffer[maxCalculationSize];

#if __EMSCRIPTEN__
// Emscripten requires buffers to be aligned
constexpr static int k_distributionAlignments[9] = {alignof(ChiSquaredDistribution),
                                                    alignof(ExponentialDistribution),
                                                    alignof(GeometricDistribution),
                                                    alignof(PoissonDistribution),
                                                    alignof(StudentDistribution),
                                                    alignof(BinomialDistribution),
                                                    alignof(FisherDistribution),
                                                    alignof(NormalDistribution),
                                                    alignof(UniformDistribution)};
constexpr static size_t k_distributionAlignment = arrayMax(k_distributionAlignments);
constexpr static int k_calculationAlignments[4] = {alignof(DiscreteCalculation),
                                                   alignof(LeftIntegralCalculation),
                                                   alignof(FiniteIntegralCalculation),
                                                   alignof(RightIntegralCalculation)};
constexpr static size_t k_calculationAlignment = arrayMax(k_calculationAlignments);
#endif

struct ProbaData {
#if __EMSCRIPTEN__
  alignas(k_distributionAlignment)
#endif
      DistributionBuffer m_distributionBuffer;
#if __EMSCRIPTEN__
  alignas(k_calculationAlignment)
#endif
      CalculationBuffer m_calculationBuffer;
};

static constexpr int statisticSizes[10] = {sizeof(OneProportionStatistic),
                                           sizeof(OneMeanZStatistic),
                                           sizeof(OneMeanTStatistic),
                                           sizeof(TwoProportionsStatistic),
                                           sizeof(TwoMeansZStatistic),
                                           sizeof(TwoMeansTStatistic),
                                           sizeof(PooledTwoMeansStatistic),
                                           sizeof(Chi2Statistic),
                                           sizeof(GoodnessStatistic),
                                           sizeof(HomogeneityStatistic)};
#if __EMSCRIPTEN__
// Emscripten requires buffers to be aligned
constexpr static int k_statisticAlignments[10] = {alignof(OneProportionStatistic),
                                                  alignof(OneMeanZStatistic),
                                                  alignof(OneMeanTStatistic),
                                                  alignof(TwoProportionsStatistic),
                                                  alignof(TwoMeansZStatistic),
                                                  alignof(TwoMeansTStatistic),
                                                  alignof(PooledTwoMeansStatistic),
                                                  alignof(Chi2Statistic),
                                                  alignof(GoodnessStatistic),
                                                  alignof(HomogeneityStatistic)};
constexpr static size_t k_statisticAlignment = arrayMax(k_statisticAlignments);
#endif

constexpr int maxStatisticSize = arrayMax(statisticSizes);
typedef char StatisticBuffer[maxStatisticSize];

struct StatisticData {
  Test m_test;
  CategoricalType m_categoricalType;
  TestType m_testType;
#if __EMSCRIPTEN__
  alignas(k_statisticAlignment)
#endif
      StatisticBuffer m_statisticBuffer;

  Statistic * statistic() { return reinterpret_cast<Statistic *>(m_statisticBuffer); }
};

union DataBuffer {
  ProbaData m_probaData;
  StatisticData m_statisticData;
};

class DataProxy {
public:
  // naive getter / setters
  ProbaData * probaData() { return &(m_dataBuffer.m_probaData); }
  StatisticData * statisticData() { return &(m_dataBuffer.m_statisticData); }

  // ProbaData
  Distribution * distribution() {
    return reinterpret_cast<Distribution *>(probaData()->m_distributionBuffer);
  }
  Calculation * calculation() {
    return reinterpret_cast<Calculation *>(probaData()->m_calculationBuffer);
  }

  // StatisticData
  Test test() { return statisticData()->m_test; }
  Test * testPointer() { return &(statisticData()->m_test); }
  TestType testType() { return statisticData()->m_testType; }
  TestType * testTypePointer() { return &(statisticData()->m_testType); }
  CategoricalType categoricalType() { return statisticData()->m_categoricalType; }
  CategoricalType * categoricalTypePointer() { return &statisticData()->m_categoricalType; }
  Statistic * statistic() { return statisticData()->statistic(); }
  HypothesisParams * hypothesisParams() { return statistic()->hypothesisParams(); }

private:
  DataBuffer m_dataBuffer;
};

}  // namespace Data
}  // namespace Probability

#endif /* PROBABILITY_MODELS_DATA_H */
