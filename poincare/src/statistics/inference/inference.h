#ifndef POINCARE_STATISTICS_INFERENCE_INFERENCE_H
#define POINCARE_STATISTICS_INFERENCE_INFERENCE_H

// Link to the public API

#include <omg/unreachable.h>
#include <poincare/comparison_operator.h>
#include <poincare/layout.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/statistics/distribution.h>

namespace Poincare::Internal {

namespace Inference {

enum class Method : bool {
  SignificanceTest,
  ConfidenceInterval,
};

constexpr int k_numberOfTestTYpes = 6;
enum class TestType : uint8_t {
  // Order matter for cells order
  OneProportion,
  OneMean,
  TwoProportions,
  TwoMeans,
  Chi2,
  Slope,
};

constexpr bool IsMethodCompatibleWithTest(Method method, TestType testType) {
  return testType != TestType::Chi2 || method != Method::ConfidenceInterval;
}
int NumberOfTestsForMethod(Method method);

enum class StatisticType : uint8_t { T, TPooled, Z, Chi2 };

enum class CategoricalType : uint8_t {
  // Order matter for cells order
  GoodnessOfFit,
  Homogeneity
};

Distribution::Type DistributionType(StatisticType statisticType);
Distribution::ParametersArray<double> DistributionParameters(
    StatisticType statisticType, double degreesOfFreedom);

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
    case TestType::Slope:
      return statisticType == StatisticType::T;
    default:
      OMG::unreachable();
  }
}

int NumberOfStatisticsForTest(TestType testType);

struct Type {
  const TestType testType;
  const StatisticType statisticType;
  const CategoricalType categoricalType;
  constexpr Type(TestType testType, StatisticType statisticType,
                 CategoricalType categoricalType)
      : testType(testType),
        statisticType(statisticType),
        categoricalType(categoricalType) {
    assert(IsTestCompatibleWithStatistic(testType, statisticType));
  }
  constexpr Type(TestType testType, StatisticType statisticType)
      : Type(testType, statisticType, CategoricalType::GoodnessOfFit) {}
  constexpr Type(CategoricalType categoricalType)
      : Type(TestType::Chi2, StatisticType::Chi2, categoricalType) {}
  constexpr Type(TestType testType)
      : Type(testType,
             IsTestCompatibleWithStatistic(testType, StatisticType::T)
                 ? StatisticType::T
                 : (IsTestCompatibleWithStatistic(testType, StatisticType::Z)
                        ? StatisticType::Z
                        : StatisticType::Chi2),
             CategoricalType::GoodnessOfFit) {}

  operator TestType() const { return testType; }
  operator StatisticType() const { return statisticType; }
  operator CategoricalType() const { return categoricalType; }
};

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
// TODO: Chi2
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
    case TestType::Chi2:
      // TODO Special case
      return -1;
    default:
      OMG::unreachable();
  }
}

// ===== Degrees of freedom =====

bool HasDegreesOfFreedom(Type type);
// Returns NAN if the type doesn't have degrees of freedom
double ComputeDegreesOfFreedom(Type type, const ParametersArray parameters);

// ===== Significance test =====

struct Hypothesis {
  double m_h0;
  Poincare::ComparisonJunior::Operator m_alternative;
  constexpr Hypothesis(double h0,
                       Poincare::ComparisonJunior::Operator alternative)
      : m_h0(h0), m_alternative(alternative) {
    assert(alternative == Poincare::ComparisonJunior::Operator::Inferior ||
           alternative == Poincare::ComparisonJunior::Operator::Superior ||
           alternative == Poincare::ComparisonJunior::Operator::NotEqual);
  }
  constexpr Hypothesis()
      : Hypothesis(0.0, Poincare::ComparisonJunior::Operator::Superior) {}
};

constexpr bool HasHyphothesis(TestType testType) {
  return testType != TestType::Chi2;
}
bool IsH0Valid(TestType testType, double h0);

constexpr int k_maxNumberOfEstimates = 3;
using Estimates = std::array<double, k_maxNumberOfEstimates>;

constexpr int NumberOfTestEstimates(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return 1;
    case TestType::TwoProportions:
      return 3;
    default:
      return 0;
  }
}

namespace EstimatesOrder {
struct OneProportion {
  enum { P };
};
struct TwoProportions {
  enum { P1, P2, Pooled };
};
};  // namespace EstimatesOrder

struct SignificanceTestResults {
  Estimates estimates;
  double criticalValue;
  double pValue;
  double degreesOfFreedom;
};
SignificanceTestResults ComputeSignificanceTest(
    Type type, Hypothesis hypothesis, const ParametersArray parameters);
Estimates ComputeTestEstimates(TestType testType,
                               const ParametersArray parameters);
double ComputeTestCriticalValue(Type type, double h0,
                                const ParametersArray parameters);
double ComputePValue(Type type, ComparisonJunior::Operator haOperator,
                     double criticalValue, double degreesOfFreedom);

// ===== Confidence interval =====

struct ConfidenceIntervalResults {
  double estimate;
  double zCritical;
  double standardError;
  double marginOfError;
  double degreesOfFreedom;
};
ConfidenceIntervalResults ComputeConfidenceInterval(
    Type type, double threshold, const ParametersArray parameters);

double ComputeIntervalCriticalValue(Type type, double threshold,
                                    double degreesOfFreedom);
bool ShowIntervalEstimate(TestType testType);
double ComputeIntervalEstimate(TestType testType,
                               const ParametersArray parameters);
double ComputeStandardError(Type type, const ParametersArray parameters);
constexpr double ComputeMarginOfError(double intervalCriticalValue,
                                      double standardError) {
  return intervalCriticalValue * standardError;
}

// ===== Layout and Symbols =====

Poincare::Layout ParameterLayout(Type type, int index);

const char* HypothesisSymbol(TestType testType);
Poincare::Layout HypothesisLayout(TestType testType);

const char* CriticalValueSymbol(StatisticType statisticType);
Poincare::Layout CriticalValueLayout(Method method,
                                     StatisticType statisticType);

Poincare::Layout TestEstimateLayout(TestType testType, int index);

const char* IntervalEstimateSymbol(TestType testType);
Poincare::Layout IntervalEstimateLayout(Type type);

// ===== Parameters =====

constexpr double DefaultThreshold(Method method) {
  return method == Method::SignificanceTest ? 0.05 : 0.95;
}
Hypothesis DefaultHyphothesis(TestType testType);

double DefaultParameterAtIndex(Method method, Type type, int index);

bool IsThresholdValid(double threshold);
bool IsParameterValidAtIndex(Type type, double p, int index);
bool AreParametersValid(Type type, const ParametersArray& parameters);

};  // namespace Inference

}  // namespace Poincare::Internal

#endif
