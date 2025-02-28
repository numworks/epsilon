#ifndef POINCARE_STATISTICS_PROBABILITY_HYPERGEOMETRIC_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_HYPERGEOMETRIC_DISTRIBUTION_H

#include <poincare/src/memory/tree.h>

#include "domain.h"
#include "omg/troolean.h"

namespace Poincare {

namespace Internal {

namespace HypergeometricDistribution {
constexpr int k_NIndex = 0;
constexpr int k_KIndex = 1;
constexpr int k_nIndex = 2;

template <typename U>
OMG::Troolean IsParameterValid(U val, int index, const U* parameters) {
  return OMG::TrooleanAnd(
      Domain::Contains(val, Domain::Type::N),
      index == k_NIndex ? OMG::Troolean::True
                        : Domain::IsAGreaterThanB(parameters[k_NIndex], val));
  ;
}

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters);

};  // namespace HypergeometricDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
