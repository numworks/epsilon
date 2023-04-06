#include "exponential_distribution.h"

#include <float.h>

#include <cmath>

namespace Distributions {

float ExponentialDistribution::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  float parameter = m_parameter;
  return parameter * std::exp(-parameter * x);
}

bool ExponentialDistribution::authorizedParameterAtIndex(double x,
                                                         int index) const {
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 7500.0) {
    return false;
  }
  return true;
}

double ExponentialDistribution::cumulativeDistributiveFunctionAtAbscissa(
    double x) const {
  if (x < 0.0) {
    return 0.0;
  }
  return 1.0 - std::exp((-m_parameter * x));
}

double ExponentialDistribution::cumulativeDistributiveInverseForProbability(
    double p) const {
  if (p >= 1.0) {
    return INFINITY;
  }
  if (p <= 0.0) {
    return 0.0;
  }
  return -std::log(1.0 - p) / m_parameter;
}

float ExponentialDistribution::privateComputeXMax() const {
  assert(m_parameter != 0.0f);
  float result = 5.0f / m_parameter;
  if (result <= FLT_EPSILON) {
    result = 1.0f;
  }
  if (std::isinf(result)) {
    // Lower privateComputeXMax. It is used for drawing so the value is not that
    // important.
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
