#include "chi_squared_distribution.h"

#include <cmath>

namespace Distributions {

bool ChiSquaredDistribution::authorizedParameterAtIndex(double x,
                                                        int index) const {
  return x <= k_maxK && std::floor(x) == x &&
         Distribution::authorizedParameterAtIndex(x, index);
}

float ChiSquaredDistribution::privateComputeXMax() const {
  assert(m_parameter != 0.0);
  return (m_parameter + 5.0f * std::sqrt(m_parameter)) *
         (1.0f + k_displayRightMarginRatio);
}

float ChiSquaredDistribution::computeYMax() const {
  float result;
  if (m_parameter / 2.0f <= 1.0f + FLT_EPSILON) {
    result = 0.5f;
  } else {
    result = evaluateAtAbscissa(m_parameter - 1.0f) * 1.2f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
