#include "hypergeometric_distribution.h"

#include <poincare/hypergeometric_distribution.h>

namespace Inference {

bool HypergeometricDistribution::authorizedParameterAtIndex(double x, int index) const {
  if (!ThreeParametersDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  return true;
}

float HypergeometricDistribution::computeXMax() const {
  return m_parameters[0] * (1.0f + k_displayRightMarginRatio);
}

float HypergeometricDistribution::computeYMax() const {
  float result = evaluateAtAbscissa(m_parameters[2]*m_parameters[1]/m_parameters[0]); // Mean is nK/N
  return result * (1.0f + k_displayTopMarginRatio);
}

}
