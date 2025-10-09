#pragma once

#include <omg/unreachable.h>
#include <poincare/comparison_operator.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/statistics/data_table.h>
#include <poincare/statistics/distribution.h>

#include <initializer_list>

namespace Poincare::Inference {

// ===== Test types =====

enum class TestType : uint8_t {
  OneProportion,
  OneMean,
  TwoProportions,
  TwoMeans,
  Chi2,
  ANOVA,
  Slope,
};

enum class StatisticType : uint8_t { T, TPooled, Z, Chi2, F };

constexpr bool IsTestCompatibleWithStatistic(TestType testType,
                                             StatisticType statisticType) {
  switch (testType) {
    case TestType::OneProportion:
    case TestType::TwoProportions:
      return statisticType == StatisticType::Z;
    case TestType::OneMean:
      return statisticType == StatisticType::T ||
             statisticType == StatisticType::Z;
    case TestType::TwoMeans:
      return statisticType == StatisticType::T ||
             statisticType == StatisticType::TPooled ||
             statisticType == StatisticType::Z;
    case TestType::Chi2:
      return statisticType == StatisticType::Chi2;
    case TestType::ANOVA:
      return statisticType == StatisticType::F;
    case TestType::Slope:
      return statisticType == StatisticType::T;
    default:
      OMG::unreachable();
  }
}

int NumberOfStatisticsForTest(TestType testType);

struct Type {
  TestType testType;
  StatisticType statisticType;
  constexpr Type(TestType testType, StatisticType statisticType)
      : testType(testType), statisticType(statisticType) {
    assert(IsTestCompatibleWithStatistic(testType, statisticType));
  }
  constexpr Type(TestType testType)
      : Type(testType,
             IsTestCompatibleWithStatistic(testType, StatisticType::T)
                 ? StatisticType::T
                 : (IsTestCompatibleWithStatistic(testType, StatisticType::Z)
                        ? StatisticType::Z
                        : ((testType == TestType::Chi2) ? StatisticType::Chi2
                                                        : StatisticType::F))) {}
  constexpr Type() : Type(TestType::OneMean) {}

  operator TestType() const { return testType; }
  operator StatisticType() const { return statisticType; }
};

// ===== Distribution =====

Distribution::Type DistributionType(StatisticType statisticType);

Distribution::ParametersArray<double> DistributionParameters(
    StatisticType statisticType, double degreesOfFreedom1,
    double degreesOfFreedom2);
/* This version is kept for retro-compatibility, it is not suitable for the F
 * statistic which needs two degreesOfFreedom parameters */
Distribution::ParametersArray<double> DistributionParameters(
    StatisticType statisticType, double degreesOfFreedom);

const char* CriticalValueSymbol(StatisticType statisticType);

// ===== Parameters =====

namespace Params {
/* We have to wrap enum in struct because enums are unscoped, so the various X,
 * N, S, etc. would conflict with each other. enum class is not an option either
 * because it doesn't allow implicit conversion to int.
 */
struct OneProportion {
  enum { X, N };
};
struct TwoProportions {
  enum { X1, N1, X2, N2 };
};
struct OneMean {
  enum { X, S, N };
};
struct TwoMeans {
  enum { X1, S1, N1, X2, S2, N2 };
};
struct Slope {
  // Number of points, Slope, Standard error
  enum { N, B, SE };
};
};  // namespace Params

constexpr int k_maxNumberOfParameters = 6;
using ParametersArray = std::array<double, k_maxNumberOfParameters>;

constexpr int NumberOfParameters(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return 2;
    case TestType::OneMean:
    case TestType::Slope:
      return 3;
    case TestType::TwoProportions:
      return 4;
    case TestType::TwoMeans:
      return 6;
    case TestType::ANOVA:
    case TestType::Chi2:
      // Special cases
      return 0;
    default:
      OMG::unreachable();
  }
}

Poincare::Layout ParameterLayout(Type type, int index);
bool IsParameterValid(Type type, double p, int index);
bool AreParametersValid(Type type, const ParametersArray& parameters);

ParametersArray ComputeOneMeanParametersFromSeries(const Series& series);
ParametersArray ComputeSlopeParametersFromSeries(const Series& series);

// ===== Degrees of freedom =====
bool HasDegreesOfFreedom(StatisticType statisticType);

// ===== Threshold =====
bool IsThresholdValid(double threshold);

namespace SignificanceTest {

struct Hypothesis {
  double m_h0;
  Poincare::Comparison::Operator m_alternative;
  constexpr Hypothesis(double h0, Poincare::Comparison::Operator alternative)
      : m_h0(h0), m_alternative(alternative) {
    assert(alternative == Poincare::Comparison::Operator::Inferior ||
           alternative == Poincare::Comparison::Operator::Superior ||
           alternative == Poincare::Comparison::Operator::NotEqual);
  }
  constexpr Hypothesis()
      : Hypothesis(0.0, Poincare::Comparison::Operator::Superior) {}
};

Hypothesis DefaultHypothesis(TestType testType);
constexpr bool HasHypothesis(TestType testType) {
  return testType != TestType::Chi2;
}
bool IsH0Valid(TestType testType, double h0);
const char* HypothesisSymbol(TestType testType);
Poincare::Layout HypothesisLayout(TestType testType);

constexpr int k_maxNumberOfEstimates = 3;
using Estimates = std::array<double, k_maxNumberOfEstimates>;

constexpr int NumberOfEstimates(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return 1;
    case TestType::TwoProportions:
      return 3;
    default:
      return 0;
  }
}

struct Results {
  Estimates estimates;
  double criticalValue;
  double pValue;
  double degreesOfFreedom;
};

Results Compute(Type type, Hypothesis hypothesis,
                const ParametersArray parameters);

Poincare::Layout CriticalValueLayout(StatisticType statisticType);
Poincare::Layout EstimateLayoutAtIndex(TestType testType, int index);

constexpr double DefaultThreshold() { return 0.05; }
double DefaultParameterAtIndex(Type type, int index);

namespace EstimatesOrder {
struct OneProportion {
  enum { P };
};
struct TwoProportions {
  enum { P1, P2, Pooled };
};
};  // namespace EstimatesOrder

namespace Chi2 {

enum class CategoricalType : uint8_t {
  // Order matter for cells order
  GoodnessOfFit,
  Homogeneity
};

bool IsObservedValueValid(double value);
bool IsExpectedValueValid(double value);
bool AreHomogeneityInputsValid(const DataTable* observedValues);
bool AreGoodnessInputsValid(const DataTable* observedValues,
                            const DataTable* expectedValues);

void FillHomogeneityExpectedValues(const DataTable* observedValues,
                                   DataTable* expectedValues);

bool IsDegreesOfFreedomValid(double p);
int ComputeDegreesOfFreedom(CategoricalType categoricalType,
                            const DataTable* observedValues);

void FillContributions(const DataTable* observedValues,
                       const DataTable* expectedValues,
                       DataTable* contributions);

Results Compute(const DataTable* contributions, double degreesOfFreedom);

}  // namespace Chi2

namespace FTest {

struct CalculatedValues {
  double sumOfSquares;
  int degreesOfFreedom;
  double meanSquares;
};

struct StatisticResults {
  double statistic;
  double pValue;
  CalculatedValues between;
  CalculatedValues within;
};

static constexpr size_t k_maxNumberOfGroups = 6;

using Values = std::initializer_list<double>;
StatisticResults ComputeStatisticResults(std::span<const Values> groups);
}  // namespace FTest

}  // namespace SignificanceTest

namespace ConfidenceInterval {

constexpr bool IsTypeCompatibleWithConfidenceInterval(TestType testType) {
  return (testType != TestType::Chi2) && (testType != TestType::ANOVA);
}

struct Results {
  double estimate;
  double zCritical;
  double standardError;
  double marginOfError;
  double degreesOfFreedom;
};

Results Compute(Type type, double threshold, const ParametersArray parameters);

bool ShowEstimate(TestType testType);

Poincare::Layout CriticalValueLayout(StatisticType statisticType);
const char* EstimateSymbol(TestType testType);
Poincare::Layout EstimateLayout(Type type);

constexpr double DefaultThreshold() { return 0.95; }
double DefaultParameterAtIndex(Type type, int index);

}  // namespace ConfidenceInterval

}  // namespace Poincare::Inference
