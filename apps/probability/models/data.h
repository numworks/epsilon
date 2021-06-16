#ifndef APPS_PROBABILITY_DATA_H
#define APPS_PROBABILITY_DATA_H

/*
 * This header defines the structures (enums/structs/typedefs/classes)
 * to store the data for the Distribution App.
 * The entrypoint is the Data class, which is meant to contain all the data,
 * and provide convenient methods to access it.
 */
#include "calculation/discrete_calculation.h"
#include "calculation/finite_integral_calculation.h"
#include "calculation/left_integral_calculation.h"
#include "calculation/right_integral_calculation.h"
#include "distribution/binomial_distribution.h"
#include "distribution/chi_squared_distribution.h"
#include "distribution/exponential_distribution.h"
#include "distribution/fisher_distribution.h"
#include "distribution/geometric_distribution.h"
#include "distribution/normal_distribution.h"
#include "distribution/poisson_distribution.h"
#include "distribution/regularized_gamma.h"
#include "distribution/student_distribution.h"
#include "distribution/uniform_distribution.h"
#include "probability/helpers.h"
#include "probability/models/hypothesis_params.h"
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

// Navigation

enum class Page {
  Menu,
  Distribution,
  Test,
  Type,
  Hypothesis,
  Categorical,
  Input,
  IntervalInput,
  InputGoodness,
  InputHomogeneity,
  Results,
  ResultsHomogeneity,
  ProbaGraph,
  Graph
};

enum class SubApp { None, Probability, Tests, Intervals };

class AppNavigation {
public:
  SubApp subapp() { return m_subApp; };
  void setSubapp(SubApp app) { m_subApp = app; };
  Page page() const { return m_page; }
  void setPage(Page page) { m_page = page; }

private:
  SubApp m_subApp;
  Page m_page;
};

// Buffers for dynamic allocation

static constexpr int distributionSizes[9] = {
    sizeof(ChiSquaredDistribution), sizeof(ExponentialDistribution), sizeof(GeometricDistribution),
    sizeof(PoissonDistribution),    sizeof(StudentDistribution),     sizeof(BinomialDistribution),
    sizeof(FisherDistribution),     sizeof(NormalDistribution),      sizeof(UniformDistribution)};

static constexpr int maxDistributionSize = arrayMax(distributionSizes);
typedef char DistributionBuffer[maxDistributionSize];

static constexpr int calculationSizes[4] = {
    sizeof(DiscreteCalculation), sizeof(FiniteIntegralCalculation), sizeof(LeftIntegralCalculation),
    sizeof(RightIntegralCalculation)};
static constexpr int maxCalculationSize = arrayMax(calculationSizes);
typedef char CalculationBuffer[maxCalculationSize];

struct ProbaData {
  DistributionBuffer m_distributionBuffer;
  CalculationBuffer m_calculationBuffer;
};

static constexpr int statisticSizes[7] = {
    sizeof(OneProportionStatistic),  sizeof(OneMeanZStatistic),  sizeof(OneMeanTStatistic),
    sizeof(TwoProportionsStatistic), sizeof(TwoMeansZStatistic), sizeof(TwoMeansTStatistic),
    sizeof(PooledTwoMeansStatistic)};

constexpr int maxStatisticSize = arrayMax(statisticSizes);
typedef char StatisticBuffer[maxStatisticSize];

// Test sub app

enum class Test { None, OneProp, OneMean, TwoProps, TwoMeans, Categorical };

inline bool isProportion(Test t) {
  return t == Test::OneProp || t == Test::TwoProps;
}

enum class TestType { TTest, PooledTTest, ZTest };

enum class CategoricalType { Goodness, Homogeneity };

constexpr static int k_maxNumberOfGoodnessInputRows = 10;
// TODO Store ?
typedef float InputGoodnessData[k_maxNumberOfGoodnessInputRows * 2];
constexpr static int k_maxNumberOfHomogeneityInputRows = 10;
constexpr static int k_maxNumberOfHomogeneityInputColumns = 10;
typedef float InputHomogeneityData[k_maxNumberOfHomogeneityInputRows]
                                  [k_maxNumberOfHomogeneityInputColumns];

// TODO store in Statistic too ?
struct CategoricalData {
  CategoricalType m_type;
  union {
    InputGoodnessData m_goodness;
    InputHomogeneityData m_homogeneity;
  } m_data;
};

struct StatisticData {
  Test m_test;
  StatisticBuffer m_statisticBuffer;
  union {
    TestType m_testType;
    CategoricalData m_categorical;
  } m_data;
  Statistic * statistic() { return reinterpret_cast<Statistic *>(m_statisticBuffer); }
};

union DataBuffer {
  StatisticData m_statisticData;
  ProbaData m_probaData;
};

class Data {
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
  void setTest(Test t) { statisticData()->m_test = t; }
  CategoricalData * categoricalData() { return &statisticData()->m_data.m_categorical; }
  TestType testType() { return statisticData()->m_data.m_testType; }
  void setTestType(TestType t) { statisticData()->m_data.m_testType = t; }
  CategoricalType categoricalType() { return categoricalData()->m_type; }
  void setCategoricalType(CategoricalType t) { categoricalData()->m_type = t; }
  InputGoodnessData * inputGoodnessData() { return &(categoricalData()->m_data.m_goodness); }
  InputHomogeneityData * inputHomogeneityData() {
    return &(categoricalData()->m_data.m_homogeneity);
  }
  Statistic * statistic() { return statisticData()->statistic(); }
  HypothesisParams * hypothesisParams() { return statistic()->hypothesisParams(); }

private:
  DataBuffer m_dataBuffer;
};

}  // namespace Data
}  // namespace Probability

#endif /* APPS_PROBABILITY_DATA_H */
