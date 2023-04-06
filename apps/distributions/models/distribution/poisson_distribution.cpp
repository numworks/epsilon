#include "poisson_distribution.h"

#include <assert.h>
#include <poincare/poisson_distribution.h>

#include <cmath>

namespace Distributions {

bool PoissonDistribution::authorizedParameterAtIndex(double x,
                                                     int index) const {
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 999.0) {
    return false;
  }
  return true;
}

float PoissonDistribution::privateComputeXMax() const {
  assert(m_parameter != 0);
  return (m_parameter + 5.0f * std::sqrt(m_parameter)) *
         (1.0f + k_displayRightMarginRatio);
}

float PoissonDistribution::computeYMax() const {
  int maxAbscissa = (int)m_parameter;
  assert(maxAbscissa >= 0.0f);
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
