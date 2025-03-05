#ifndef POINCARE_STATISTICS_PROBABILITY_NORMAL_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_NORMAL_DISTRIBUTION_H

#include <poincare/src/memory/tree.h>

#include "domain.h"

namespace Poincare {

namespace Internal {

namespace NormalDistribution {
constexpr int k_muIndex = 0;
constexpr int k_sigmaIndex = 1;

template <typename U>
OMG::Troolean IsParameterValid(U val, int index, const U* parameters) {
  return index == k_muIndex ? Domain::Contains(val, Domain::Type::R)
                            : Domain::Contains(val, Domain::Type::RPlusStar);
  ;
}

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters);

template <typename T>
T MeanAbscissa(const T* parameters) {
  return parameters[k_muIndex];
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters);

double EvaluateParameterForProbabilityAndBound(int parameterIndex,
                                               const double* parameters,
                                               double probability, double bound,
                                               bool isUpperBound);

constexpr static double k_standardMu = 0.;
constexpr static double k_standardSigma = 1.;

};  // namespace NormalDistribution

}  // namespace Internal

}  // namespace Poincare

#endif
