#ifndef POINCARE_STATISTICS_INFERENCE_CONFIDENCE_INTERVAL_H
#define POINCARE_STATISTICS_INFERENCE_CONFIDENCE_INTERVAL_H

#include "inference.h"

namespace Poincare::Internal {

namespace Inference {

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

// ===== PRIVATE =====

double ComputeCriticalValue(Type type, double threshold,
                            double degreesOfFreedom);
double ComputeEstimate(TestType testType, const ParametersArray parameters);
double ComputeStandardError(Type type, const ParametersArray parameters);
constexpr double ComputeMarginOfError(double intervalCriticalValue,
                                      double standardError) {
  return intervalCriticalValue * standardError;
}

}  // namespace ConfidenceInterval

};  // namespace Inference

}  // namespace Poincare::Internal

#endif
