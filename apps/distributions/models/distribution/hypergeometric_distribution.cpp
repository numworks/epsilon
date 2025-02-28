#include "hypergeometric_distribution.h"

namespace Distributions {

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
  float maximum = std::max(evaluateAtAbscissa(std::floor(mean)),
                           evaluateAtAbscissa(std::ceil(mean)));
  return maximum * (1.0f + k_displayTopMarginRatio);
}

}  // namespace Distributions
