#include "fisher_distribution.h"

#include <float.h>
#include <poincare/layout.h>

#include <cmath>

using namespace Shared;

namespace Distributions {

bool FisherDistribution::authorizedParameterAtIndex(double x, int index) const {
  assert(index == 0 || index == 1);
  return Distribution::authorizedParameterAtIndex(x, index) && x > DBL_MIN &&
         x <= k_maxParameter;
}

float FisherDistribution::mode() const {
  const float d1 = m_parameters[0];
  if (d1 > 2.0f) {
    const float d2 = m_parameters[1];
    return (d1 - 2.0f) / d1 * d2 / (d2 + 2.0f);
  }
  return NAN;
}

float FisherDistribution::privateComputeXMax() const {
  return 5.0f;  // The mode is always < 1
}

float FisherDistribution::computeYMax() const {
  const float m = mode();
  float max = std::isnan(m) ? k_defaultMax : evaluateAtAbscissa(m);
  max = std::isnan(max) ? k_defaultMax : max;
  return max * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
