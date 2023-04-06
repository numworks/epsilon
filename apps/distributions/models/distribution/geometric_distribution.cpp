#include "geometric_distribution.h"

#include <assert.h>
#include <poincare/geometric_distribution.h>

#include <cmath>

namespace Distributions {

bool GeometricDistribution::authorizedParameterAtIndex(double x,
                                                       int index) const {
  assert(index == 0);
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 1.0) {
    return false;
  }
  return true;
}

float GeometricDistribution::privateComputeXMax() const {
  assert(m_parameter != 0.0f);
  return 5.0f / m_parameter * (1.0f + k_displayRightMarginRatio);
}

float GeometricDistribution::computeYMax() const {
  // Tha distribution is max for x == 1
  float result = evaluateAtAbscissa(1.0);
  return result * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
