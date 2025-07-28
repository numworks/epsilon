#include "uniform_distribution.h"

#include <assert.h>
#include <float.h>
#include <poincare/layout.h>

#include <algorithm>
#include <cmath>

using namespace Shared;

namespace Distributions {

float UniformDistribution::evaluateAtAbscissa(float t) const {
  float parameter1 = m_parameters[0];
  float parameter2 = m_parameters[1];
  /* This is a hack to have better rendering of the distributions when the
   * parameters are very close */
  if (parameter2 - parameter1 < FLT_EPSILON) {
    if (parameter1 - k_diracWidth <= t && t <= parameter2 + k_diracWidth) {
      return 2.0f * k_diracMaximum;
    }
    return 0.0f;
  }
  return Distribution::evaluateAtAbscissa(t);
}

void UniformDistribution::setParameterAtIndex(double f, int index) {
  setParameterAtIndexWithoutComputingCurveViewRange(f, index);
  if (index == 0 && m_parameters[1] < m_parameters[0]) {
    // Add more than 1.0 if first parameter is greater than 100.
    m_parameters[1] =
        m_parameters[0] +
        std::max(1.0, std::round(std::fabs(m_parameters[0]) * 0.01));
  }
  computeCurveViewRange();
}

}  // namespace Distributions
