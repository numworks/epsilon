#ifndef POINCARE_STATISTICS_PROBABILITY_DISCRETE_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_DISCRETE_DISTRIBUTION_H

#include <poincare/statistics/distribution.h>

namespace Poincare {

namespace Internal {

// More precisely distributions deriving from this should be defined on N
namespace DiscreteDistribution {
template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(
    Distribution::Type distribType, T x,
    const Distribution::ParametersArray<T> parameters);

// The range is inclusive on both ends
template <typename T>
T CumulativeDistributiveFunctionForRange(
    Distribution::Type distribType, T x, T y,
    const Distribution::ParametersArray<T> parameters) {
  if (y < x) {
    return 0.0f;
  }
  return Distribution::CumulativeDistributiveFunctionAtAbscissa(distribType, y,
                                                                parameters) -
         Distribution::CumulativeDistributiveFunctionAtAbscissa(
             distribType, x - 1.0f, parameters);
}
};  // namespace DiscreteDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
