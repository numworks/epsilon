#ifndef DATA_H
#define DATA_H

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

namespace Probability {
namespace Data {

// Navigation

enum class Page {
  Menu = 0,
  Distribution = 1,
  Test = 2,
  Type = 3,
  Hypothesis = 4,
  Categorical = 5,
  Input = 6,
  InputGoodness = 7,
  InputHomogeneity = 8,
  Results = 9,
  ResultsHomogeneity = 10,
  ProbaGraph = 11,
  Graph = 12
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

constexpr static int max(int a, int b) {
  return a > b ? a : b;
}

constexpr static int max_between(const int * begin, const int * end) {
  return begin + 1 == end ? *begin : max_between(begin + 1, end);
}

template<int N>
constexpr static int arrayMax(const int (&data)[N]) {
  return max_between(data, data + N);
}

static constexpr int distributionSizes[9] = {
    sizeof(ChiSquaredDistribution), sizeof(ExponentialDistribution), sizeof(GeometricDistribution),
    sizeof(PoissonDistribution),    sizeof(StudentDistribution),     sizeof(BinomialDistribution),
    sizeof(FisherDistribution),     sizeof(NormalDistribution),      sizeof(UniformDistribution)};

static constexpr int maxDistributionSize = arrayMax(distributionSizes);
typedef char DistributionBuffer[maxDistributionSize];

static constexpr int calculationSizes[4] = {sizeof(DiscreteCalculation), sizeof(FiniteIntegralCalculation),
                                           sizeof(LeftIntegralCalculation), sizeof(RightIntegralCalculation)};
static constexpr int maxCalculationSize = arrayMax(calculationSizes);
typedef char CalculationBuffer[maxCalculationSize];

struct ProbaData {
  DistributionBuffer m_distributionBuffer;
  CalculationBuffer m_calculationBuffer;
};

// Test sub app

enum class Test {
  OneProp, OneMean, TwoProps, TwoMeans, Categorical
};

enum class TestType {
  TTest, PooledTTest, ZTest
};

enum class ComparisonOperator {
  Lower, Higher, Different
};

struct HypothesisParams {
public:
  float firstParam() const { return m_firstParam; }
  void setFirstParam(float firstParam) { m_firstParam = firstParam; }

  ComparisonOperator op() const { return m_op; }
  void setOp(const ComparisonOperator op) { m_op = op; }

private:
  float m_firstParam;
  ComparisonOperator m_op;
};

constexpr int k_maxNumberOfInputParameters = 3 * 2;  // t-test on two means (mean, std, size)
class InputParameters {
public:
  float paramAtIndex(int i) const { return m_params[i]; }
  void setParamAtIndex(int i, float p) { m_params[i] = p; }
  float significanceLevel() const { return m_significanceLevel; }
  void setSignificanceLevel(float s) { m_significanceLevel = s; }

private:
  float m_params[k_maxNumberOfInputParameters];
  float m_significanceLevel;
};

enum class CategoricalType {
  Goodness, Homogeneity
};

constexpr static int k_maxNumberOfGoodnessInputRows = 10;
// TODO Store ?
typedef float InputGoodnessData[k_maxNumberOfGoodnessInputRows * 2];
constexpr static int k_maxNumberOfHomogeneityInputRows = 10;
constexpr static int k_maxNumberOfHomogeneityInputColumns = 10;
typedef float InputHomogeneityData[k_maxNumberOfHomogeneityInputRows][k_maxNumberOfHomogeneityInputColumns];

struct CategoricalData {
  CategoricalType m_type;
  union {
    InputGoodnessData m_goodness;
    InputHomogeneityData m_homogeneity;
  } m_data;
};

struct TestData {
  TestType m_testType;
  HypothesisParams m_hypothesisParams;
  InputParameters m_inputParams;
};

struct TestIntervalData {
  Test m_test;
  union {
    TestData m_test;
    CategoricalData m_categorical;
  } m_data;
};

constexpr static int dataSizes[] = {sizeof(ProbaData), sizeof(TestIntervalData)};
constexpr static int maxDataSize = arrayMax(dataSizes);

typedef char DataBuffer[maxDataSize];

class Data {
public:
  // naive getter / setters
  ProbaData * probaData() { return reinterpret_cast<ProbaData *>(&m_buffer); }
  TestIntervalData * testIntervalData() { return reinterpret_cast<TestIntervalData *>(&m_buffer); }

  // ProbaData
  Distribution * distribution() { return reinterpret_cast<Distribution *>(probaData()->m_distributionBuffer); }
  Calculation * calculation() { return reinterpret_cast<Calculation *>(probaData()->m_calculationBuffer); }

  // TestIntervalData
  Test test() { return testIntervalData()->m_test; }
  void setTest(Test t) { testIntervalData()->m_test = t; }
  TestData * testData() { return &testIntervalData()->m_data.m_test; }
  CategoricalData * categoricalData() { return &testIntervalData()->m_data.m_categorical; }
  TestType testType() { return testData()->m_testType; }
  HypothesisParams hypothesisParams() { return testData()->m_hypothesisParams; }
  InputParameters testInputParams() { return testData()->m_inputParams; }
  void setTestType(TestType t) { testData()->m_testType = t; }
  CategoricalType categoricalType() { return categoricalData()->m_type; }
  void setCategoricalType(CategoricalType t) { categoricalData()->m_type = t; }
  InputGoodnessData * inputGoodnessData() { return &(categoricalData()->m_data.m_goodness); }
  InputHomogeneityData * inputHomogeneityData() { return &(categoricalData()->m_data.m_homogeneity); }

private:
  DataBuffer m_buffer;
};

}  // namespace Data
}  // namespace Probability

#endif /* DATA_H */
