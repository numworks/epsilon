#pragma once

#include "inference.h"

namespace Poincare::Internal::Inference::ConfidenceInterval {

double ComputeCriticalValue(Inference::Type type, double threshold,
                            double degreesOfFreedom);
double ComputeEstimate(TestType testType, const ParametersArray parameters);
double ComputeStandardError(Inference::Type type,
                            const ParametersArray parameters);
constexpr double ComputeMarginOfError(double intervalCriticalValue,
                                      double standardError) {
  return intervalCriticalValue * standardError;
}

}  // namespace Poincare::Internal::Inference::ConfidenceInterval
