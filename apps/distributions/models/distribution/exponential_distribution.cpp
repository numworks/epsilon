#include "exponential_distribution.h"

#include <float.h>

#include <cmath>

#include "distributions/models/distribution/distribution.h"

namespace Distributions {

bool ExponentialDistribution::authorizedParameterAtIndex(double x,
                                                         int index) const {
  return Distribution::authorizedParameterAtIndex(x, index) && x > 0.0 &&
         x <= 7500.0;
}

float ExponentialDistribution::privateComputeXMax() const {
  assert(m_parameter != 0.0f);
  float result = 5.0f / m_parameter;
  if (result <= FLT_EPSILON) {
    result = 1.0f;
  }
  if (std::isinf(result)) {
    /* Lower privateComputeXMax. It is used for drawing so the value is not that
     * important. */
    return 1.0f / m_parameter;
  }
  return result * (1.0f + k_displayRightMarginRatio);
}

float ExponentialDistribution::computeYMax() const {
  float result = m_parameter;
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
