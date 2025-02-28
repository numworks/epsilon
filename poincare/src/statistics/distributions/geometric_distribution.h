#ifndef POINCARE_STATISTICS_PROBABILITY_GEOMETRIC_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_GEOMETRIC_DISTRIBUTION_H

#include <poincare/src/memory/tree.h>

#include "domain.h"

namespace Poincare {

namespace Internal {

/* We chose the definition:
 * 0 < p <= 1 for distribution of success
 * k number of trials needed to get one success, where k ∈ {1, 2, 3, ...}. */

namespace GeometricDistribution {
template <typename U>
OMG::Troolean IsParameterValid(U val, int index, const U* parameters) {
  return Domain::Contains(val, Domain::Type::ZeroExcludedToOne);
}

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters);

};  // namespace GeometricDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
