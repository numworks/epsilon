#ifndef POINCARE_STATISTICS_PROBABILITY_CHI2_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_CHI2_DISTRIBUTION_H

#include <omg/troolean.h>
#include <poincare/src/memory/tree.h>

#include "domain.h"

namespace Poincare {

namespace Internal {

namespace Chi2Distribution {
template <typename U>
OMG::Troolean IsParameterValid(U val, int index, const U* parameters) {
  return Domain::Contains(val, Domain::Type::NStar);
}

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* parameters);

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters);

};  // namespace Chi2Distribution

}  // namespace Internal

}  // namespace Poincare

#endif
