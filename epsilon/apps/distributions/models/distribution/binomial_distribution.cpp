#include "binomial_distribution.h"

#include <assert.h>
#include <poincare/layout.h>

#include <cmath>

using namespace Shared;

namespace Distributions {

double BinomialDistribution::rightIntegralInverseForProbability(
    double p) const {
  if (m_parameters[0] == 0.0 &&
      (m_parameters[1] == 0.0 || m_parameters[1] == 1.0)) {
    return NAN;
  }
  if (p <= 0.0) {
    return m_parameters[0];
  }
  return Distribution::rightIntegralInverseForProbability(p);
}

}  // namespace Distributions
