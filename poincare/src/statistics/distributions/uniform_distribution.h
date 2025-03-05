#ifndef POINCARE_STATISTICS_PROBABILITY_UNIFORM_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_UNIFORM_DISTRIBUTION_H

#include <poincare/src/memory/tree.h>

#include "domain.h"
#include "omg/troolean.h"

namespace Poincare {

namespace Internal {

namespace UniformDistribution {
template <typename U>
OMG::Troolean IsParameterValid(U val, int index, const U* parameters) {
  return OMG::TrooleanAnd(Domain::Contains(val, Domain::Type::R),
                          index == 0
                              // d1 <= d2
                              ? Domain::IsAGreaterThanB(parameters[1], val)
                              : Domain::IsAGreaterThanB(val, parameters[0]));
}

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters);

template <typename T>
T MeanAbscissa(const T* parameters) {
  return (parameters[0] + parameters[1]) / 2.0;
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters);

constexpr static double k_diracWidth = 0.005;
constexpr static double k_diracMaximum = 10.0;

};  // namespace UniformDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
