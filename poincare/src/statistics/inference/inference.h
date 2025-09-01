#pragma once

#include <poincare/statistics/inference.h>

namespace Poincare::Internal::Inference {
using namespace Poincare::Inference;

// Returns NAN if the type doesn't have degrees of freedom
double ComputeDegreesOfFreedom(Inference::Type type,
                               const ParametersArray parameters);

double TwoMeansStandardError(StatisticType statisticType,
                             const ParametersArray parameters);

}  // namespace Poincare::Internal::Inference
