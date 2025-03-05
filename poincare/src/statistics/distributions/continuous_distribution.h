#ifndef POINCARE_STATISTICS_PROBABILITY_CONTINUOUS_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_CONTINUOUS_DISTRIBUTION_H

#include <poincare/statistics/distribution.h>

namespace Poincare {

namespace Internal {

namespace ContinuousDistribution {
// The range is inclusive on both ends
template <typename T>
T CumulativeDistributiveFunctionForRange(
    Distribution::Type distribType, T x, T y,
    const Distribution::ParametersArray<T> parameters) {
  if (y <= x) {
    return 0.0f;
  }
  return Distribution::CumulativeDistributiveFunctionAtAbscissa(distribType, y,
                                                                parameters) -
         Distribution::CumulativeDistributiveFunctionAtAbscissa(distribType, x,
                                                                parameters);
}

/* This method looks for bounds such that:
 * cumulativeDistributionEvaluation(xmin) < 0 <
 * cumulativeDistributionEvaluation(xmax)
 */
template <typename T>
void FindBoundsForBinarySearch(
    typename Solver<T>::FunctionEvaluation cumulativeDistributionEvaluation,
    const void* auxiliary, T& xmin, T& xmax);

};  // namespace ContinuousDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
