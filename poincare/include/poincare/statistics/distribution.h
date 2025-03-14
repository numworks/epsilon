#ifndef POINCARE_STATISTICS_DISTRIBUTION_H
#define POINCARE_STATISTICS_DISTRIBUTION_H

#include <poincare/src/statistics/distributions/distribution.h>

namespace Poincare {

namespace Distribution {

using Internal::Distribution::GetType;
using Internal::Distribution::Type;

namespace Params = Internal::Distribution::Params;

constexpr int k_maxNumberOfParameters =
    Internal::Distribution::k_maxNumberOfParameters;

using Internal::Distribution::DefaultParameterAtIndex;
using Internal::Distribution::NumberOfParameters;
using Internal::Distribution::ParameterNameAtIndex;
using Internal::Distribution::ParametersArray;

using Internal::Distribution::IsContinuous;
using Internal::Distribution::IsSymmetrical;

using Internal::Distribution::AreParametersValid;
using Internal::Distribution::IsParameterValid;

using Internal::Distribution::AcceptsOnlyPositiveAbscissa;
using Internal::Distribution::CumulativeDistributiveFunctionAtAbscissa;
using Internal::Distribution::CumulativeDistributiveFunctionForRange;
using Internal::Distribution::CumulativeDistributiveInverseForProbability;
using Internal::Distribution::EvaluateAtAbscissa;
using Internal::Distribution::MeanAbscissa;

using Internal::Distribution::EvaluateParameterForProbabilityAndBound;

}  // namespace Distribution

}  // namespace Poincare

#endif
