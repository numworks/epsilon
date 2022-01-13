#include "geometric_distribution.h"

#include <assert.h>
#include <cmath>

namespace Probability {

bool GeometricDistribution::authorizedValueAtIndex(double x, int index) const {
  assert(index == 0);
  if (!OneParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 1.0) {
    return false;
  }
  return true;
}

template<typename T>
T GeometricDistribution::templatedApproximateAtAbscissa(T k) const {
  constexpr T castedOne = static_cast<T>(1.0);
  if (k < castedOne) {
    return static_cast<T>(0.0);
  }
  T p = static_cast<T>(m_parameter1);
  if (p == castedOne) {
    return k == castedOne ? castedOne : static_cast<T>(0.0);
  }
  // The result is p * (1-p)^{k-1}
  T lResult = (k - castedOne) * std::log(castedOne - p);
  return p * std::exp(lResult);
}

float GeometricDistribution::computeXMin() const {
  return -k_displayLeftMarginRatio * computeXMax();
}

float GeometricDistribution::computeXMax() const {
  assert(m_parameter1 != 0.0f);
  return 5.0f/m_parameter1 * (1.0f + k_displayRightMarginRatio);
}

float GeometricDistribution::computeYMax() const {
  float result = evaluateAtAbscissa(1.0); // Tha distribution is max for x == 1
  return result * (1.0f + k_displayTopMarginRatio);
}

}

template float Probability::GeometricDistribution::templatedApproximateAtAbscissa(float x) const;
template double Probability::GeometricDistribution::templatedApproximateAtAbscissa(double x) const;
