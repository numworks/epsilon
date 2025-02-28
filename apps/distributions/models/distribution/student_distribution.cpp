#include "student_distribution.h"

namespace Distributions {

bool StudentDistribution::authorizedParameterAtIndex(double x,
                                                     int index) const {
  // We cannot draw the curve for x > 200 (coefficient() is too small)
  return Distribution::authorizedParameterAtIndex(x, index) &&
         x >= DBL_EPSILON && x <= 200.0;
}

float StudentDistribution::privateComputeXMin() const {
  return -privateComputeXMax();
}

float StudentDistribution::privateComputeXMax() const { return 5.0f; }

float StudentDistribution::computeYMax() const {
  const float floatParam = static_cast<float>(m_parameter);
  return m_distribution.evaluateAtAbscissa(0.0f, &floatParam) *
         (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
