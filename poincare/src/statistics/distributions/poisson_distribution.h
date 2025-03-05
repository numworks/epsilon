#ifndef POINCARE_STATISTICS_PROBABILITY_POISSON_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_POISSON_DISTRIBUTION_H

#include <poincare/src/memory/tree.h>
#include <poincare/statistics/distribution.h>

#include "domain.h"

namespace Poincare {

namespace Internal {

namespace PoissonDistribution {
template <typename U>
OMG::Troolean IsParameterValid(
    U val, int index, const Distribution::ParametersArray<U> parameters) {
  return Domain::Contains(val, Domain::Type::RPlusStar);
}

template <typename T>
T EvaluateAtAbscissa(T x, const Distribution::ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(
    T probability, const Distribution::ParametersArray<T> parameters);

};  // namespace PoissonDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
