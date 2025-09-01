#pragma once

#include <poincare/statistics/inference.h>

#include "inference.h"

namespace Poincare::Internal::Inference::SignificanceTest {

// WARNING: Chi2 is in another file (chi2_test.h)

double ComputePValue(StatisticType statisticType,
                     Comparison::Operator haOperator, double criticalValue,
                     double degreesOfFreedom);
Poincare::Inference::SignificanceTest::Estimates ComputeEstimates(
    TestType testType, const ParametersArray parameters);
double ComputeCriticalValue(Type type, double h0,
                            const ParametersArray parameters);

}  // namespace Poincare::Internal::Inference::SignificanceTest
