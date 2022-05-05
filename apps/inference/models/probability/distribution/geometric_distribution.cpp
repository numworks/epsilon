#include "geometric_distribution.h"

#include <poincare/geometric_distribution.h>
#include <assert.h>
#include <cmath>

namespace Inference {

bool GeometricDistribution::authorizedParameterAtIndex(double x, int index) const {
  assert(index == 0);
  if (!OneParameterDistribution::authorizedParameterAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 1.0) {
    return false;
  }
  return true;
}

template<typename T>
T GeometricDistribution::templatedApproximateAtAbscissa(T k) const {
  return Poincare::GeometricDistribution::EvaluateAtAbscissa<T>(k, static_cast<T>(m_parameter));
}

float GeometricDistribution::computeXMax() const {
  assert(m_parameter != 0.0f);
  return 5.0f/m_parameter * (1.0f + k_displayRightMarginRatio);
}

float GeometricDistribution::computeYMax() const {
  float result = evaluateAtAbscissa(1.0); // Tha distribution is max for x == 1
  return result * (1.0f + k_displayTopMarginRatio);
}

template float GeometricDistribution::templatedApproximateAtAbscissa(float x) const;
template double GeometricDistribution::templatedApproximateAtAbscissa(double x) const;

}
