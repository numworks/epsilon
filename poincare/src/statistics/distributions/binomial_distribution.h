#ifndef POINCARE_STATISTICS_PROBABILITY_BINOMIAL_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_BINOMIAL_DISTRIBUTION_H

#include <omg/troolean.h>
#include <poincare/src/memory/tree.h>

#include "domain.h"

namespace Poincare {

namespace Internal {

namespace BinomialDistribution {

constexpr int k_nIndex = 0;
constexpr int k_pIndex = 1;

template <typename T>
OMG::Troolean IsParameterValid(T val, int index, const T* parameters) {
  return index == k_nIndex ? Domain::Contains(val, Domain::Type::N)
                           : Domain::Contains(val, Domain::Type::ZeroToOne);
}

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters);

};  // namespace BinomialDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
