#ifndef POINCARE_STATISTICS_PROBABILITY_CONTINUOUS_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_CONTINUOUS_DISTRIBUTION_H

#include <poincare/statistics/distribution.h>

namespace Poincare {

namespace Internal {

namespace ContinuousDistribution {
// The range is inclusive on both ends
template <typename T>
T CumulativeDistributiveFunctionForRange(Distribution::Type distribType, T x,
                                         T y, const T* parameters) {
  Distribution distribution(distribType);
  return distribution.cumulativeDistributiveFunctionAtAbscissa(y, parameters) -
         distribution.cumulativeDistributiveFunctionAtAbscissa(x, parameters);
}

};  // namespace ContinuousDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
