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

// ===== Test types =====

constexpr int k_numberOfTestTypes = 6;
enum class TestType : uint8_t {
  // Order matter for cells order
  OneProportion,
  OneMean,
  TwoProportions,
  TwoMeans,
  Chi2,
  Slope,
};

enum class StatisticType : uint8_t { T, TPooled, Z, Chi2 };

enum class CategoricalType : uint8_t {
  // Order matter for cells order
  GoodnessOfFit,
  Homogeneity
};

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

// ===== Distribution =====

Distribution::Type DistributionType(StatisticType statisticType);
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
    case TestType::Chi2:
      // Special case
      return 0;
    default:
      OMG::unreachable();
  }
}

Poincare::Layout ParameterLayout(Type type, int index);
bool IsParameterValidAtIndex(Type type, double p, int index);
bool AreParametersValid(Type type, const ParametersArray& parameters);

// ===== Degrees of freedom =====

bool HasDegreesOfFreedom(Type type);
// Returns NAN if the type doesn't have degrees of freedom
double ComputeDegreesOfFreedom(Type type, const ParametersArray parameters);

// ===== Threshold =====

bool IsThresholdValid(double threshold);

// ===== PRIVATE =====

double TwoMeansStandardError(StatisticType statisticType,
                             const ParametersArray parameters);

};  // namespace Inference

}  // namespace Poincare::Internal

#endif
