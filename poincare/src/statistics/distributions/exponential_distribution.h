#ifndef POINCARE_STATISTICS_PROBABILITY_EXPONENTIAL_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_EXPONENTIAL_DISTRIBUTION_H

#include <omg/troolean.h>
#include <poincare/src/memory/tree.h>
#include <poincare/statistics/distribution.h>

#include "domain.h"

namespace Poincare {

namespace Internal {

namespace ExponentialDistribution {
template <typename U>
OMG::Troolean IsParameterValid(
    U val, int index, const Distribution::ParametersArray<U> parameters) {
  return Domain::Contains(val, Domain::Type::RPlusStar);
}

template <typename T>
T EvaluateAtAbscissa(T x, const Distribution::ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(
    T x, const Distribution::ParametersArray<T> parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(
    T probability, const Distribution::ParametersArray<T> parameters);

};  // namespace ExponentialDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
