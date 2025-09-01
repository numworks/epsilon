#pragma once

#include <poincare/src/statistics/inference/inference.h>

namespace Poincare {

namespace Inference {

using Internal::Inference::StatisticType;
using Internal::Inference::TestType;

constexpr int k_numberOfTestTypes = Internal::Inference::k_numberOfTestTypes;

using Internal::Inference::IsTestCompatibleWithStatistic;
using Internal::Inference::NumberOfStatisticsForTest;

using Internal::Inference::Type;

using Internal::Inference::CriticalValueSymbol;
using Internal::Inference::DistributionParameters;
using Internal::Inference::DistributionType;

namespace Params = Internal::Inference::Params;

using ParametersArray = Internal::Inference::ParametersArray;

constexpr int k_maxNumberOfParameters =
    Internal::Inference::k_maxNumberOfParameters;
using Internal::Inference::AreParametersValid;
using Internal::Inference::IsParameterValid;
using Internal::Inference::NumberOfParameters;
using Internal::Inference::ParameterLayout;

using Internal::Inference::ComputeOneMeanParametersFromSeries;
using Internal::Inference::ComputeSlopeParametersFromSeries;

using Internal::Inference::HasDegreesOfFreedom;

using Internal::Inference::IsThresholdValid;

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

}  // namespace SignificanceTest

namespace ConfidenceInterval {

constexpr bool IsTypeCompatibleWithConfidenceInterval(TestType testType) {
  return testType != TestType::Chi2;
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

}  // namespace Inference

}  // namespace Poincare
