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

}  // namespace Distributions
