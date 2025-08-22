#pragma once

#include "inference.h"

namespace Poincare::Internal {

namespace Inference {

namespace SignificanceTest {

// WARNING: Chi2 is in another file (chi2_test.h)

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

namespace EstimatesOrder {
struct OneProportion {
  enum { P };
};
struct TwoProportions {
  enum { P1, P2, Pooled };
};
};  // namespace EstimatesOrder

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

// ===== PRIVATE =====

double ComputePValue(StatisticType statisticType,
                     Comparison::Operator haOperator, double criticalValue,
                     double degreesOfFreedom);
Estimates ComputeEstimates(TestType testType, const ParametersArray parameters);
double ComputeCriticalValue(Type type, double h0,
                            const ParametersArray parameters);

}  // namespace SignificanceTest

};  // namespace Inference

}  // namespace Poincare::Internal
