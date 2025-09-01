#pragma once

#include <poincare/src/statistics/inference/confidence_interval.h>
#include <poincare/src/statistics/inference/inference.h>
#include <poincare/src/statistics/inference/significance_test.h>

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

using Internal::Inference::SignificanceTest::HasHypothesis;
using Internal::Inference::SignificanceTest::Hypothesis;
using Internal::Inference::SignificanceTest::HypothesisLayout;
using Internal::Inference::SignificanceTest::HypothesisSymbol;
using Internal::Inference::SignificanceTest::IsH0Valid;

using Internal::Inference::SignificanceTest::Estimates;
using Internal::Inference::SignificanceTest::NumberOfEstimates;

namespace EstimatesOrder =
    Internal::Inference::SignificanceTest::EstimatesOrder;

using Internal::Inference::SignificanceTest::Compute;
using Internal::Inference::SignificanceTest::Results;

using Internal::Inference::SignificanceTest::CriticalValueLayout;
using Internal::Inference::SignificanceTest::EstimateLayoutAtIndex;

using Internal::Inference::SignificanceTest::DefaultHypothesis;
using Internal::Inference::SignificanceTest::DefaultParameterAtIndex;
using Internal::Inference::SignificanceTest::DefaultThreshold;

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

using Internal::Inference::ConfidenceInterval::
    IsTypeCompatibleWithConfidenceInterval;

using Internal::Inference::ConfidenceInterval::Compute;
using Internal::Inference::ConfidenceInterval::Results;

using Internal::Inference::ConfidenceInterval::ShowEstimate;

using Internal::Inference::ConfidenceInterval::CriticalValueLayout;
using Internal::Inference::ConfidenceInterval::EstimateLayout;
using Internal::Inference::ConfidenceInterval::EstimateSymbol;

using Internal::Inference::ConfidenceInterval::DefaultParameterAtIndex;
using Internal::Inference::ConfidenceInterval::DefaultThreshold;

}  // namespace ConfidenceInterval

}  // namespace Inference

}  // namespace Poincare
