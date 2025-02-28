#ifndef POINCARE_STATISTICS_PROBABILITY_DISCRETE_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_DISCRETE_DISTRIBUTION_H

#include <poincare/statistics/distribution.h>

namespace Poincare {

namespace Internal {

// More precisely distributions deriving from this should be defined on N
namespace DiscreteDistribution {
template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(Distribution::Type distribType, T x,
                                           const T* parameters);

// The range is inclusive on both ends
template <typename T>
T CumulativeDistributiveFunctionForRange(Distribution::Type distribType, T x,
                                         T y, const T* parameters) {
  return CumulativeDistributiveFunctionAtAbscissa(distribType, y, parameters) -
         CumulativeDistributiveFunctionAtAbscissa(distribType, x - 1.0f,
                                                  parameters);
}
};  // namespace DiscreteDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
