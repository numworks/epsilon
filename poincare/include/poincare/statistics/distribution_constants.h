#ifndef POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_CONSTANTS_H
#define POINCARE_STATISTICS_PROBABILITY_DISTRIBUTION_CONSTANTS_H

#include <poincare/src/statistics/distributions/normal_distribution.h>
#include <poincare/src/statistics/distributions/uniform_distribution.h>

namespace Poincare::DistributionConstants {

namespace UniformDistribution {

constexpr static double k_diracWidth =
    Internal::UniformDistribution::k_diracWidth;
constexpr static double k_diracMaximum =
    Internal::UniformDistribution::k_diracMaximum;

}  // namespace UniformDistribution

}  // namespace Poincare::DistributionConstants

#endif
