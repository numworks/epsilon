#include "hypergeometric_distribution.h"

#include <poincare/hypergeometric_distribution.h>

namespace Distributions {

bool HypergeometricDistribution::authorizedParameterAtIndex(double x,
                                                            int index) const {
  if (!ThreeParametersDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (!std::isfinite(x) || std::floor(x) != x || x < 0) {
    return false;
  }
  if (index == 0) {  // N
    return x > 0;
  }
  // K or n
  return x <= m_parameters[0];
}

void HypergeometricDistribution::setParameterAtIndex(double f, int index) {
  setParameterAtIndexWithoutComputingCurveViewRange(f, index);
  if (index == 0) {
    m_parameters[1] = std::min(m_parameters[0], m_parameters[1]);
    m_parameters[2] = std::min(m_parameters[0], m_parameters[2]);
  }
  computeCurveViewRange();
}

float HypergeometricDistribution::privateComputeXMax() const {
  return std::min(m_parameters[1], m_parameters[2]) *
         (1.0f + k_displayRightMarginRatio);
}

float HypergeometricDistribution::computeYMax() const {
  float mean =
      m_parameters[2] * m_parameters[1] / m_parameters[0];  // n * K / N
  float maximum = evaluateAtAbscissa(mean);
  return maximum * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
